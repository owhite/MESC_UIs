#! /usr/bin/env python3

import os, io
import threading
from datetime import datetime
import plotMESC

import gspread
from oauth2client.service_account import ServiceAccountCredentials

from google.oauth2 import service_account
from googleapiclient.discovery import build
from google.auth.exceptions import GoogleAuthError
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.http import MediaIoBaseUpload

class handler:
    def __init__(self, logger=None, account_file=None):

        self.logger = logger
        # Define the Google Drive API scopes and service account file path
        SCOPES = ['https://www.googleapis.com/auth/drive']

        # Create credentials using the service account file
        self.credentials = service_account.Credentials.from_service_account_file(account_file, scopes=SCOPES)

        # Build the Google Drive service
        self.drive_service = build('drive', 'v3', credentials=self.credentials)


    def create_folder(self, folder_name, parent_folder_id=None):
        """Create a folder in Google Drive and return its ID."""
        folder_metadata = {
            'name': folder_name,
            "mimeType": "application/vnd.google-apps.folder",
            'parents': [parent_folder_id] if parent_folder_id else []
        }

        created_folder = self.drive_service.files().create(body=folder_metadata, fields='id').execute()

        return created_folder["id"]

    def get_folder_id(self, name):
        results = self.drive_service.files().list(
            q=f"trashed=false", 
            pageSize=1000,
            fields="nextPageToken, files(id, name, mimeType)"
        ).execute()
        items = results.get('files', [])

        if not items:
            if self.logger:
                self.logger.info("No folders or files found in Google Drive.")
        else:
            for item in items:
                if item['name'] == name:
                    return(item['id'])

        return(None)


    def list_files(self, parent_folder_id=None, delete=False):
        """List folders and files in Google Drive."""
        results = self.drive_service.files().list(
            q=f"'{parent_folder_id}' in parents and trashed=false" if parent_folder_id else None,
            pageSize=1000,
            fields="nextPageToken, files(id, name, mimeType, parents)"
        ).execute()
        items = results.get('files', [])

        l = []
        if items:
            for item in items:
                if delete:
                    self.delete_files(item['id'])
                else:
                    l.append(item)
        return(l)

    def delete_files(self, file_or_folder_id):
        """Delete a file or folder in Google Drive by ID."""
        try:
            self.drive_service.files().delete(fileId=file_or_folder_id).execute()
            if self.logger:
                self.logger.info(f"Successfully deleted file/folder with ID: {file_or_folder_id}")
        except Exception as e:
            if self.logger:
                self.logger.info(f"Error deleting file/folder with ID: {file_or_folder_id}")
                self.logger.info(f"Error details: {str(e)}")

    def download_file(self, file_id, destination_path):
        """Download a file from Google Drive by its ID."""
        request = self.drive_service.files().get_media(fileId=file_id)
        fh = io.FileIO(destination_path, mode='wb')
    
        downloader = MediaIoBaseDownload(fh, request)
    
        done = False
        while not done:
            status, done = downloader.next_chunk()
            if self.logger:
                self.logger.info(f"Download {int(status.progress() * 100)}%.")

    def upload_file(self, file_path, file_name, mime_type='application/octet-stream', folder_id=None):
        if folder_id is not None:
            file_metadata = {
                'name': file_name,
                'parents': [folder_id]  # Specify the folder ID
            }
        else:
            file_metadata = {'name': file_name}

        media = MediaIoBaseUpload(io.FileIO(file_path, 'rb'), mimetype=mime_type)

        # Create file on Google Drive
        file = self.drive_service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    
        # Get URL for the file
        file_id = file.get('id')
        file_url = f'https://drive.google.com/file/d/{file_id}/view?usp=sharing'

        return file_id, file_url
    
    def set_permissions(self, file_id):
        # set permission to wide open
        permission = {
            'role': 'reader',
            'type': 'anyone',
            'allowFileDiscovery': False,
        }
        if self.logger:
            self.logger.info(f"Setting permissions on {file_id}")

        self.drive_service.permissions().create(fileId=file_id, body=permission).execute()

    def add_row_to_spreadsheet(self, plot_url, data_url, timestamp, note):
        # Define the scope and credentials
        scope = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']
        client = gspread.authorize(self.credentials)

        # Use a pre-existing spreadsheet, here
        # https://docs.google.com/spreadsheets/d/1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c/
        spreadsheet_id = '1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c'
        worksheet_name = 'MESC_UPLOADS'  # Change to your worksheet name

        try:
            spreadsheet = client.open_by_key(spreadsheet_id)
            worksheet = client.open_by_key(spreadsheet_id).worksheet(worksheet_name)
            # Add a new row of data to the end of the worksheet
            new_data = [plot_url, data_url, timestamp, note]
            worksheet.append_row(new_data, value_input_option='USER_ENTERED')

            if self.logger:
                self.logger.info(f"Added row to spreadsheet with id = {spreadsheet_id}")

        except Exception as e:
            self.logger.info(f"Error attempting row addition to spreadsheet_id = {spreadsheet_id}")
            return None

class uploadThread(threading.Thread):
    def __init__(self, parent, files, note):
        super().__init__()
        self.parent = parent
        self.status_label = self.parent.status_label
        self.logger = self.parent.msgs
        self.drive = self.parent.drive
        self.files = files
        self.note = note
        self.stopped = threading.Event()
        self.plot_file = files

    def run(self):
        for file_path in self.files:
            if not os.path.exists(file_path):
                self.status_label.setText(F"{file_path} missing")
                self.logger.logger.info(f"{file_path} missing")
                self.stop()
                return()

        self.status_label.setText(F"Upload log file")
        file_url = self.uploadToDrive(self.files[0], 'thing.txt')
        self.status_label.setText(F"Upload plot")
        plot_url = self.uploadToDrive(self.files[1], 'thing.png')
        self.status_label.setText(F"Insert into spreadsheet")

        u1 = (F"=hyperlink(\"{plot_url}\", \"PLOT\")")
        u2 = (F"=hyperlink(\"{file_url}\", \"DATA\")")
        today = datetime.today()
        current_time = datetime.now()
        military_time = current_time.strftime("%H:%M")
        formatted_date = today.strftime("%m-%d-%y") + " " + military_time
        self.drive.add_row_to_spreadsheet(u1, u2, formatted_date, self.note)
        self.status_label.setText(F"Done with upload")
        for file_path in self.files:
            try:
                os.remove(file_path)
                self.logger.logger.info(f"File '{file_path}' deleted successfully.")
            except OSError as e:
                self.logger.logger.info(f"Error deleting the file '{file_path}': {e}")
                self.delete(file_path)
        self.stop()

    def stop(self):
        self.stopped.set()

    def uploadToDrive(self, output_file, dest_name):
        self.logger.logger.info("Uploading initiated")

        # List folders and files
        l = self.drive.list_files()

        want_to_delete = False
        for item in l:
            if item['name'] == dest_name and want_to_delete:
                self.drive.delete_files(item['id']) # good for testing
                self.logger.logger.info(F"DELETING {item} {item['id']}")

        file_id, file_url =  self.drive.upload_file(output_file, dest_name)
        self.logger.logger.info(F"file url {file_url}")

        self.drive.set_permissions(file_id)

        return(file_url)

if __name__ == '__main__':
    # Example usage:

    drive = handler()
    # Create a new folder
    # drive.create_folder("MESC_DATA")

    # Download a file by its ID
    # drive.download_file("your_file_id", "destination_path/file_name.extension")

    # drive.delete_files('1t0MReXDzX_iIiIzlgMpa2HkFqCHJIq_A')

    # folder_id = drive.get_folder_id("MESC_DATA")

    file_path = '/Users/owhite/thing.txt'
    dest_name = 'thing.txt'

    file_id, file_url =  drive.upload_file(file_path, dest_name)

    # List folders and files
    l = drive.list_files()

    for item in l:
        print(item)
        if item['name'] == dest_name:
            print ("found and deleting ", dest_name)
            drive.delete_files(item['id'])

    file_id, file_url =  drive.upload_file(file_path, dest_name)
    drive.set_permissions(file_id)

