/*
 * retroTerm widget test sketch
 * 
 * This sketch tests all the widget types, one at a time. Code for each type of widget is in a separate tab.
 * 
 * Probing terminal size, to see what space is available. You can change terminal size between tests to check this works.
 * Button test. The buttons of different style appear and and can be clicked with mouse or keyboard shortcut.
 * Checkbox test. Three differet checkboxes appear and can be checked with the mouse or keyboard shortcut.
 * Radio button test. Three different radio buttons can be appear and one can be selected with the mouse or keyboard shortcut.
 * Fixed text display test. Three windows of wordwrapped text, they can be clicked on and will scroll with the mouse wheel or arrow keys on the keyboard.
 * Scrolling text display test. Three windows of text, which scroll up as more is added.
 * List box test. Three list boxes with different options that can be selected in each.
 * Text input field test. Three text input fields, which can be selected with the mouse or keyboard shortcuts. You can type in these with normal editing keys.
 * 
 */
 #include <retroTerm.h>    //Include terminal library

retroTerm terminal;       //Create a terminal instance

//Global variables for the widgets

uint8_t red = 0;
uint8_t green = 0;
uint8_t blue = 0;

void setup() {
  Serial.begin(115200);   //Initialise the Serial stream
  terminal.begin(Serial); //Initialise the library
  terminal.defaultAttributes(BACKGROUND_COLOR_BLACK | COLOUR_WHITE);  //Set some initial attributes for printing
  terminal.probeSize();   //Attempts to determine how many lines and columns the terminal has
}

void loop() {
  textInputExample(); 
}

/*
 * The 'newPage' function uses some 'primitves' from the library
 * to clear the screen and show a title at the top of the page before every test
 */

void newPage(String title) {
  terminal.reset();             //Resets the terminal, clearing colours etc.
  terminal.hideCursor();        //Hide the cursor
  terminal.printAt(1,1,title + ", press a key or wait 30s to finish");  //Print the title at the top of the page
  terminal.println();           //Print a blank line
  terminal.setScrollWindow(3,terminal.lines()/2 - 4);     //Set up somewhere to show the events without disrupting the widgets
  terminal.enableMouse();       //Enable the mouse
}

/*
 * The 'endPage' function tidies up after the test
 */
void endPage() {
  terminal.houseKeeping();
  if(terminal.keyPressed())
  {
    terminal.readKeypress();
  }
  terminal.deleteWidget(red);     //It is safe to try and delete a non-existent widget, the method will check before attempting to de-allocate memory etc.
  terminal.deleteWidget(green);
  terminal.deleteWidget(blue);
  terminal.houseKeeping();
}



void textInputExample()
{
  newPage(F("Text input field test"));
  red = terminal.newTextInput(terminal.columns()/4,  terminal.lines()/2 - 2, terminal.columns()/2, 1, F("Red:"),   COLOUR_WHITE | BACKGROUND_COLOUR_RED, SHORTCUT_INLINE);    //Create three new widgets and record the IDs
  green = terminal.newTextInput(terminal.columns()/4, terminal.lines()/2, terminal.columns()/2, 4, F("Green"), COLOUR_GREEN, OUTER_BOX);  //The method returns 0 if it fails
  blue = terminal.newTextInput(terminal.columns()/4, terminal.lines()/2 + 4, terminal.columns()/2, 5, F("Blue (Password)"),  COLOUR_BLUE, OUTER_BOX | BOX_DOUBLE_LINE | LABEL_IN_BOX | LABEL_CENTRED | SHORTCUT_INLINE | PASSWORD_FIELD);   //Colour is the colour of the frame, lettering can be different
  if(red)
  {
    terminal.labelAttributes(red, COLOUR_WHITE | ATTRIBUTE_BRIGHT | BACKGROUND_COLOUR_RED);     //Make the label text more emphasised, for better contrast
    terminal.scroll(F("Red text input field created"));
    terminal.showWidget(red);                                               //Make this widget visible
    terminal.widgetShortcutKey(red,f10Pressed);
    terminal.setWidgetContent(red,F("Red text input"));                     //Populate the field at the start
  }
  if(green)
  {
    terminal.labelAttributes(green, COLOUR_GREEN | ATTRIBUTE_BRIGHT); //Make the label text more emphasised, for better contrast
    terminal.scroll(F("Green text input field created"));
    terminal.showWidget(green);                                       //Make this widget visible
    terminal.widgetShortcutKey(green,f11Pressed);
    terminal.setWidgetContent(green,F("Green text input"));           //Populate the field at the start
  }
  if(blue)
  {
    terminal.labelAttributes(blue, COLOUR_BLUE | ATTRIBUTE_BRIGHT);    //Make the label text more emphasised, for better contrast
    terminal.scroll(F("Blue text input field created"));
    terminal.showWidget(blue);                                         //Make this widget visible
    terminal.widgetShortcutKey(blue,f12Pressed);
    terminal.setWidgetContent(blue,F("Blue password input"));          //Populate the field at the start
  }
  terminal.scroll(F("Press enter when finished to see the inputs"));   //It is for the application to decide what to do with everything that isn't a line editing key
  uint32_t timeout = millis();
  while(millis() - timeout < 30000ul && not terminal.keyPressed())
  {
    if(terminal.widgetClicked(red))
    {
      terminal.scroll(F("Red text input field selected"));
    }
    if(terminal.widgetClicked(green))
    {
      terminal.scroll(F("Green text input field selected"));
    }
    if(terminal.widgetClicked(blue))
    {
      terminal.scroll(F("Blue text input field selected"));
    }
    terminal.houseKeeping();  //You MUST run housekeeping to show any changes!
  }
  if(red && terminal.contentChanged(red))
  {
    terminal.scroll("Red string changed - " + String(terminal.retrieveContent(red)));
  }
  if(green && terminal.contentChanged(green))
  {
    terminal.scroll("Green string changed - " + String(terminal.retrieveContent(green)));
  }
  if(blue && terminal.contentChanged(blue))
  {
    terminal.scroll("Blue string changed - " + String(terminal.retrieveContent(blue)));
  }
  terminal.deselectWidget();  //Deselect the currently typed in widget, to prevent more typing
  if(red)
  {
    terminal.widgetPassive(red);  //Stop the widget being clicked on again
  }
  if(green)
  {
    terminal.widgetPassive(green);  //Stop the widget being clicked on again
  }
  if(blue)
  {
    terminal.widgetPassive(blue);  //Stop the widget being clicked on again
  }
  terminal.houseKeeping();  //You MUST run housekeeping to show any changes!
  if(terminal.keyPressed())
  {
    terminal.readKeypress();    //Gobble up the enter key so the script will wait to show the strings
  }
  timeout = millis();
  while(millis() - timeout < 30000ul && not terminal.keyPressed())
  {
    terminal.houseKeeping();  //You MUST run housekeeping to show any changes!
  }
  if(terminal.keyPressed())
  {
    terminal.readKeypress();  //Gobble up the keypress so the script will wait for the next page
  }
  endPage();
}
