import argparse
import numpy as np
from PIL import Image
import os

# python png_to_lvgl.py shadow_buttons.png button_off button_off.h scrap2.png 66 34 110 74 60;
# python png_to_lvgl.py shadow_buttons.png button_on button_on.h scrap2.png 66 34 110 74 60;


def rgb_to_rgb565(r, g, b):
    """
    Convert 8-bit RGB values to 16-bit RGB565 format.
    """
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def generate_lvgl_header(image_data, image_name, width, height, output_file):
    """
    Generate a C header file for LVGL from the extracted 16-bit RGB565 image data.
    """
    header = f'#include <lvgl.h>\n\n'
    header += f'const uint16_t {image_name}_map[] = {{\n    '

    # Convert each pixel to RGB565 format and add it to the header
    hex_values = [f'0x{pixel:04X}' for pixel in image_data.flatten()]

    # Group the values into lines with 8 values per line
    hex_lines = [', '.join(hex_values[i:i + 8]) for i in range(0, len(hex_values), 8)]

    # Join the lines and add them to the header
    header += ',\n    '.join(hex_lines)
    header += '\n};\n\n'
    
    header += f'const lv_img_dsc_t {image_name} = {{\n'
    header += f'    .header = {{\n'
    header += f'        .cf = LV_IMG_CF_TRUE_COLOR,\n'
    header += f'        .w = {width},\n'
    header += f'        .h = {height}\n'
    header += f'    }},\n'
    header += f'    .data_size = sizeof({image_name}_map),\n'
    header += f'    .data = (const uint8_t *){image_name}_map\n'
    header += f'}};\n'

    # Write to file
    with open(output_file, 'w') as f:
        f.write(header)
    print(f"Header file generated: {output_file}")

def extract_image(input_image, image_name, x, y, width, height, reduction_percent):
    """
    Extract a portion of the image starting from (x, y) with the specified width and height.
    Resize it based on the reduction percentage and convert to 16-bit RGB565 format.
    """
    # Open image and convert to RGB (if not already in that mode)
    img = Image.open(input_image).convert('RGB')
    img_cropped = img.crop((x, y, x + width, y + height))

    # Calculate new dimensions based on the reduction percentage
    new_width = int(width * (reduction_percent / 100))
    new_height = int(height * (reduction_percent / 100))

    # Resize the image based on the reduction percentage
    img_resized = img_cropped.resize((new_width, new_height), Image.LANCZOS)

    # Convert resized image to 16-bit RGB565
    img_rgb = np.array(img_resized)
    img_rgb565 = np.array([[rgb_to_rgb565(r, g, b) for r, g, b in row] for row in img_rgb], dtype=np.uint16)

    return img_resized, image_name, img_rgb565, new_width, new_height

def save_extracted_png(image, output_png):
    """
    Save the extracted image as a new PNG file.
    """
    image.save(output_png)
    print(f"Extracted region saved as: {output_png}")

def main():
    # Command-line argument parsing
    parser = argparse.ArgumentParser(description='Generate LVGL header file and save extracted PNG from an input PNG image.')
    parser.add_argument('input_image', help='Path to the input PNG image')
    parser.add_argument('image_name', help='named used in header file')
    parser.add_argument('output_header', help='Path to output header file (e.g., image1.h)')
    parser.add_argument('output_png', help='Path to output extracted PNG file (e.g., extracted_image.png)')
    parser.add_argument('x', type=int, help='X coordinate of the top-left corner to start extracting')
    parser.add_argument('y', type=int, help='Y coordinate of the top-left corner to start extracting')
    parser.add_argument('width', type=int, help='Width of the portion to extract')
    parser.add_argument('height', type=int, help='Height of the portion to extract')
    parser.add_argument('reduction_percent', type=int, help='Percentage to reduce the size of the extracted image (0-100)')

    args = parser.parse_args()

    # Validate input image file
    if not os.path.isfile(args.input_image):
        print(f"Error: Input image '{args.input_image}' does not exist.")
        return

    # Validate the reduction percentage
    if args.reduction_percent < 0 or args.reduction_percent > 100:
        print(f"Error: Reduction percentage must be between 0 and 100.")
        return

    # Extract the image portion and resize it based on the reduction percentage
    img_resized, image_name, image_data, new_width, new_height = extract_image(args.input_image, args.image_name, args.x, args.y, args.width, args.height, args.reduction_percent)

    # Save the resized extracted region as a PNG file
    save_extracted_png(img_resized, args.output_png)

    # Generate the header file for the resized image
    generate_lvgl_header(image_data, image_name, new_width, new_height, args.output_header)

if __name__ == '__main__':
    main()
