#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtk.h>

static float num1 = 0;
static char lastChar = (char) 0;
static char prevCmd = (char) 0;
#define BUF_SIZE 88

typedef struct {

    char      *szLabel;    // Label display on button
    int       row;         // Row to place the button
    int       col;         // Column to place the button
    GtkWidget *widget;     // Handle to the button

} typCalculatorButton;


typCalculatorButton buttonList [] = {
    {"C",   1, 0, NULL},      /* --- Clear --- */
    {"CE",  1, 1, NULL},      /* --- Clear --- */
    {"/",   1, 3, NULL},      /* --- Division --- */

    {"7",   2, 0, NULL},      /* --- Digit --- */
    {"8",   2, 1, NULL},      /* --- Digit --- */
    {"9",   2, 2, NULL},      /* --- Digit --- */
    {"*",   2, 3, NULL},      /* --- Multiplication --- */
    {"%",   2, 4, NULL},      /* --- Percent --- */

    {"4",   3, 0, NULL},      /* --- Digit --- */
    {"5",   3, 1, NULL},      /* --- Digit --- */
    {"6",   3, 2, NULL},      /* --- Digit --- */
    {"-",   3, 3, NULL},      /* --- Subtraction --- */
    {"1/x", 3, 4, NULL},      /* --- 1/x --- */

    {"1",   4, 0, NULL},      /* --- Digit --- */
    {"2",   4, 1, NULL},      /* --- Digit --- */
    {"3",   4, 2, NULL},      /* --- Digit --- */
    {"+",   4, 3, NULL},      /* --- Addition --- */
    {"sqrt",4, 4, NULL},      /* --- Square root --- */

    {"+/-", 5, 0, NULL},      /* --- Negate value --- */
    {"0",   5, 1, NULL},      /* --- zero --- */
    {".",   5, 2, NULL},      /* --- Decimal --- */
    {"=",   5, 3, NULL},      /* --- Equals/total --- */
    {"x^2", 5, 4, NULL},      /* --- Squared --- */
};

int nButtons = sizeof (buttonList) / sizeof (typCalculatorButton);

// Results panel
GtkWidget *label;

void trimTrailingZeros (char *szDigits) {
    int nIndex;
    int bDecimal = FALSE;
    int nPos = -1;

    // Foreach char in string
    for (nIndex = 0; nIndex < strlen (szDigits); nIndex++) {

        if (szDigits[nIndex] == '.') {
             bDecimal = TRUE;
        }

        // If right of decimal point
        if (bDecimal) {

            /* --- A zero?  Hmm... from this point on? --- */
            if (szDigits[nIndex] == '0') {

               /* --- If we don't have a point yet... --- */
               if (nPos < 0) {
 
                   /* --- Save this as a point. --- */
                   nPos = nIndex;
               }
            } else {

               /* --- Clear it.  Bad point. --- */
               nPos = -1;
            }
        }
    }

    /* --- Truncate the field. --- */
    if (nPos > 0) {
        szDigits[nPos] = (char) 0;
    }
}


void trimLeadingZeros (char *szDigits) {
    int nPos;

    if (szDigits == NULL) return;

    /* --- While we have a combination a digit in front --- */
    for (nPos = 0; (szDigits[nPos] && szDigits[nPos] == '0'); nPos++) {

        /* --- If the digit is a zero and next char is a digit --- */
        if (isdigit (szDigits[nPos+1])) {

            /* --- Blank the field. --- */  
            szDigits[nPos] = ' ';
        } 
    }
}


int command (char ch) {
    switch (ch) {
        case '+':
        case '-':
        case '/':
        case '*':
        case '=':
            return (TRUE);
    }
    return (FALSE);
}

int floatingPointChar (char ch) {
    return (isdigit (ch) || ch == '.');
}

// Pass on to button click event
void key_press (GtkWidget *widget, GdkEventKey *event, gpointer data) {
    int nIndex;

    /* --- Search through the buttons --- */
    for (nIndex = 0; nIndex < nButtons; nIndex++) {

        /* --- If the keystroke is the first character of a button AND --- */
        /* --- the button label length is one. --- */      
        if (event->keyval == buttonList[nIndex].szLabel[0] && 
            buttonList[nIndex].szLabel[1] == (char) 0) {

            /* --- Set focus to that button --- */
            gtk_widget_grab_focus (buttonList[nIndex].widget);

            /* --- Make like the button was clicked to do processing. --- */
            gtk_button_clicked (GTK_BUTTON (buttonList[nIndex].widget));
            return;
        }
    }
}


void handleDigit (char *str, char ch) {
    const char *labelText;
    char buffer[BUF_SIZE];
    int  len;

    /* --- And they just did a command --- */
    if (command (lastChar)) {

        /* --- Clear the digit field --- */
        gtk_label_set_text (GTK_LABEL (label), "");

        /* --- If they did a computation --- */
        if (lastChar == '=') {

            /* --- Clear out the command --- */
            lastChar = (char) 0;
            prevCmd = (char) 0;
        }
    }

    /* --- Get the buffer in the led --- */
    labelText = gtk_label_get_text (GTK_LABEL (label));
    strcpy (buffer, labelText);

    /* --- Add the new character on it. --- */
    len = strlen (buffer);
    buffer[len] = (gchar) ch;
    buffer[len+1] = (gchar) 0;
   
    /* --- Trim leading zeros. --- */
    trimLeadingZeros (buffer);

    /* --- Add digit to field. --- */
    gtk_label_set_text (GTK_LABEL (label), (char *) buffer);
}

// Do now not later?
void maybeUnaryOperation (char *str) {
    const char *labelText;
    char buffer[BUF_SIZE];
    float num2;

    // --- Get number in the field. 
    labelText = gtk_label_get_text (GTK_LABEL (label));
    num2 = atof (labelText);

    // --- Percentage?
    if (strcmp (str, "%") == 0) {
        num2 = num2 / 100;

    // --- Trying for 1/x?
    } else if (strcmp (str, "1/x") == 0) {

        // --- Can't divide by zero.
        if (num2 == 0) {
            return;
        }
        num2 = 1 / num2;

    // --- Calculate sqrt
    } else if (strcmp (str, "sqrt") == 0) {
        num2 = sqrt ((double) num2);

    // --- Calculate square
    } else if (strcmp (str, "x^2") == 0) {
        num2 = num2 * num2;

    // --- Calculate Negation
    } else if (strcmp (str, "+/-") == 0) {
        num2 = 0 - num2;
        prevCmd = (char) 0;
        lastChar = (char) 0;
    }

    /* --- Put the number back. --- */
    sprintf (buffer, "%f", (float) num2);
    trimTrailingZeros (buffer);
    trimLeadingZeros (buffer);
    gtk_label_set_text (GTK_LABEL (label), buffer);
}


void handleBinaryOperation () {
    char buffer[BUF_SIZE];
    const char *labelText;
    float num2;

    /* --- Get number in the field. --- */
    labelText = gtk_label_get_text (GTK_LABEL (label));
    num2 = atof (labelText);

    /* --- Calculate based on previous command. --- */
    switch (prevCmd) {
        case '+':
            num1 = num1 + num2;               
            break;

        case '-':
            num1 = num1 - num2;               
            break;

        case '*':
            num1 = num1 * num2;               
            break;

        case '/':
            num1 = num1 / num2;               
            break;

        case '=':
            num1 = num2;
            break;

        default:
            num1 = num2;
            break;
    }

    /* --- Put the number back. --- */
    sprintf (buffer, "%f", (float) num1);
    trimTrailingZeros (buffer);
    trimLeadingZeros (buffer);
    gtk_label_set_text (GTK_LABEL (label), buffer);
}


/*
 * button_clicked
 *
 * widget - button pressed.
 * data - button label.
 *
 * Button was pressed, handle it.
*/
void button_clicked (GtkWidget *widget, gpointer data) {
    char ch = *((char *) data);
    char *str;
    
    /* --- Get the button label --- */
    str = (char *) data;

    /* --- Entering a number... --- */
    if (floatingPointChar (ch) && strlen (str) == 1) {

        handleDigit (str, ch);

    } else {

        /* --- Clear? --- */
        if (strcmp (str, "CE") == 0) {
            gtk_label_set_text (GTK_LABEL (label), "0");
            return;

        /* --- BIG clear? --- */
        } else if (strcmp (str, "C") == 0) {
            prevCmd = (char) 0;
            lastChar = (char) 0;
            gtk_label_set_text (GTK_LABEL (label), "0");
            return;

        } else {

            /* --- Maybe it's a unary operator? --- */
            maybeUnaryOperation (str);
        }

        /* --- See if there's a binary operation to do --- */
        handleBinaryOperation ();

        prevCmd = ch;
    }
    lastChar = ch;
}

/*
 * CreateButton
 *
 * Create a button, assign event handlers, and attach the button to the
 * table in the proper place.
*/
GtkWidget *createButton (GtkWidget *table, char *szLabel, int row, int column) {
    GtkWidget *button;

    /* --- Create the button --- */
    button = gtk_button_new_with_label (szLabel);

    /* --- We care if the button is clicked --- */
    g_signal_connect (G_OBJECT (button), "clicked",
                        G_CALLBACK (button_clicked), szLabel);

    /* --- Put the button in the table in the right place. --- */
    gtk_grid_attach (GTK_GRID (table), button, 
            column, row,
            1, 1);

    /* --- Make the button visible --- */
    gtk_widget_show (button);

    /* --- return the button. --- */
    return (button);
}



/*
 * CreateCalculatorButtons
 *
 * Create the buttons on the calculator from the table we defined at the
 * beginning of this program.  The button pointers (handles) are stored
 * back in the table so they can be referenced later.
*/
void createCalculatorButtons (GtkWidget *table) {
    int nIndex;

    /* --- Run through the list of buttons. --- */
    for (nIndex = 0; nIndex < nButtons; nIndex++) {

        /* --- Create a button --- */
        buttonList[nIndex].widget = 
                createButton (table, 
                              buttonList[nIndex].szLabel, 
                              buttonList[nIndex].row, 
                              buttonList[nIndex].col);
    }
}

/*
 * main
 *
 * Program begins here
*/
int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *table;

    /* --- GTK initialization --- */
    gtk_init (&argc, &argv);

    /* --- Create the calculator window --- */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* --- Give the window a title. --- */
    gtk_window_set_title (GTK_WINDOW (window), "Calculator");

    /* --- Set the window size. --- */
    gtk_widget_set_size_request (window, 200, 200);

    /* --- We care if a key is pressed --- */
    g_signal_connect (G_OBJECT (window), "key-press-event",
                        G_CALLBACK (key_press), NULL);

    /* --- You should always remember to connect the delete event
     *     to the main window. --- */
    g_signal_connect (G_OBJECT (window), "destroy",
                        G_CALLBACK (gtk_main_quit), NULL);

    /* --- Create a 5x5 table for the items in the calculator. --- */
    table = gtk_grid_new (); 
    gtk_grid_set_column_homogeneous(GTK_GRID (table), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID (table), TRUE);

    /* --- Create the calculator buttons. --- */
    createCalculatorButtons (table);

    /* --- Create the calculator LED --- */
    label = gtk_label_new ("0");
    gtk_widget_set_halign(GTK_WIDGET (label), GTK_ALIGN_END);
    gtk_widget_set_valign(GTK_WIDGET (label), GTK_ALIGN_CENTER);

    /* --- Add label to the table --- */
    gtk_grid_attach (GTK_GRID (table), label, 
                              0, 0, 5, 1);
    gtk_widget_show (label);
  
    /* --- Make them visible --- */
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show (table);
    gtk_widget_show (window);

    /* --- Grab focus for the keystrokes --- */
    //gtk_widget_grab_focus (buttonList[0].widget);

    gtk_main ();
    return (0);
}



