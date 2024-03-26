#!/usr/bin/env python3

import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QLabel, QLineEdit
from PyQt5.QtCore import Qt, QEvent


class FourButtonWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.init_ui()

    def init_ui(self):
        # Create the label to display button click information
        self.label = QLabel('Click a button', self)
        self.label.setAlignment(Qt.AlignCenter)

        # Create the four buttons with larger font size
        self.button1 = QPushButton('Button 1', self)
        self.button1.setFixedSize(200, 60)
        self.button1.setFocusPolicy(Qt.StrongFocus)
        self.button1.clicked.connect(self.button1_clicked)

        self.button2 = QPushButton('Button 2', self)
        self.button2.setFixedSize(200, 60)
        self.button2.setFocusPolicy(Qt.StrongFocus)
        self.button2.clicked.connect(self.button2_clicked)

        self.button3 = QPushButton('Button 3', self)
        self.button3.setFixedSize(200, 60)
        self.button3.setFocusPolicy(Qt.StrongFocus)
        self.button3.clicked.connect(self.button3_clicked)

        self.button4 = QPushButton('Button 4', self)
        self.button4.setFixedSize(200, 60)
        self.button4.setFocusPolicy(Qt.StrongFocus)
        self.button4.clicked.connect(self.button4_clicked)

        # Create a line edit for text input (hidden by default)
        self.line_edit = QLineEdit(self)
        self.line_edit.setPlaceholderText("Enter text here")
        self.line_edit.returnPressed.connect(self.save_text)
        self.line_edit.hide()

        # Create a vertical layout for buttons
        button_layout = QVBoxLayout()
        button_layout.addWidget(self.button1)
        button_layout.addWidget(self.button2)
        button_layout.addWidget(self.button3)
        button_layout.addWidget(self.button4)
        button_layout.addStretch(1)  # Add stretch to move label to bottom

        # Create a vertical layout for the window
        layout = QVBoxLayout(self)
        layout.addWidget(self.label)
        layout.addWidget(self.line_edit)
        layout.addLayout(button_layout)

        # Set window geometry
        self.setGeometry(100, 100, 400, 300)
        self.setWindowTitle('Four Large Buttons')

        # Install event filter to handle key events
        self.installEventFilter(self)

    def eventFilter(self, obj, event):
        if event.type() == QEvent.KeyPress:
            key = event.key()
            if key == Qt.Key_Down:
                self.move_focus(1)
                return True
            elif key == Qt.Key_Up:
                self.move_focus(-1)
                return True
            elif key == Qt.Key_Return:
                self.trigger_button()
                return True
        return super().eventFilter(obj, event)

    def move_focus(self, direction):
        focus_widget = QApplication.focusWidget()
        buttons = [self.button1, self.button2, self.button3, self.button4]
        current_index = buttons.index(focus_widget) if focus_widget in buttons else -1
        new_index = (current_index + direction) % len(buttons)
        buttons[new_index].setFocus()

    def trigger_button(self):
        focused_button = QApplication.focusWidget()
        if focused_button == self.button1:
            self.button1_clicked()
        elif focused_button == self.button2:
            self.button2_clicked()
        elif focused_button == self.button3:
            self.button3_clicked()
        elif focused_button == self.button4:
            self.button4_clicked()

    def button1_clicked(self):
        self.label.setText("Button 1 Clicked")

    def button2_clicked(self):
        self.label.setText("Button 2 Clicked")

    def button3_clicked(self):
        self.label.setText("Button 3 Clicked")

    def button4_clicked(self):
        if self.line_edit.isHidden():
            self.line_edit.show()
            self.button4.setText("Save Text")
        else:
            self.save_text()

    def save_text(self):
        text = self.line_edit.text()
        self.label.setText(f"Text entered: {text}")
        self.line_edit.clear()
        self.line_edit.hide()
        self.button4.setText("Button 4")


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = FourButtonWindow()
    window.show()
    sys.exit(app.exec_())
