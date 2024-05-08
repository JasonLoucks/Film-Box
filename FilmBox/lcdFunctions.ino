// print a message at the specified location
void lcdPrintAtPos( char msg[], int col, int row )
{
    lcd.setCursor( col, row );
    lcd.print( msg );
}

// clear a single character at the specified location
void lcdClearChar( int col, int row )
{
    lcd.setCursor( col, row );
    lcd.print( " " ); // overwrite the current character with a space
}