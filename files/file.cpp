 
#include <windows.h>

 
int wmain (void)
{
  HANDLE hFile, hAppend;
  DWORD dwBytesRead, dwBytesWritten, dwPos;
  char buff[4096];
  TCHAR szMsg[1000];

 
  // Open the existing file.
          printf("\n\n\n*****************************************\n");
            printf("Toradex AG\n");
            printf("*****************************************\n");
			
  
  hFile = CreateFile (TEXT("\\ONE.txt"),      // Open One.txt
                      GENERIC_READ,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_ALWAYShola,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  //if(hFile < 0)
  {
    // Your error-handling code goes here.
    printf ("Could not open ONE.txt");
	getchar();
    return FALSE;
  }

  // Open the existing file, or, if the file does not exist,
  // create a new file.

  hAppend = CreateFile (TEXT("\\TWO.TXT"),      // Open Two.txt.
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

  if (hAppend == INVALID_HANDLE_VALUE)
  {
    printf("Could not open TWO.TXT") ;
    CloseHandle (hFile);            // Close the first file.
	getchar();
    return FALSE;
  }

  // Append the first file to the end of the second file.

  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
  do
  {
    if (ReadFile (hFile, buff, 4096, &dwBytesRead, NULL))
    {
      WriteFile (hAppend, buff, dwBytesRead,
                 &dwBytesWritten, NULL);
    }
  }
  while (dwBytesRead == 4096);

  // Close both files.

  CloseHandle (hFile);
  CloseHandle (hAppend);

  return TRUE;
} // End of AppendExample code