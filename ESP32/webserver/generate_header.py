import os

def generate_header():
    input_file = 'data/index.html'  # Path to your HTML file
    output_file = 'src/index_html.h'  # Path to the output header file

    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found.")
        return

    with open(input_file, 'r') as f:
        html_content = f.read()

    header_content = f"""
    #ifndef INDEX_HTML_H
    #define INDEX_HTML_H

    #include <pgmspace.h>

    const char index_html[] PROGMEM = R"rawliteral(
    {html_content}
    )rawliteral";

    #endif
    """

    with open(output_file, 'w') as f:
        f.write(header_content)

    print(f'Header file generated: {output_file}')

if __name__ == '__main__':
    generate_header()

