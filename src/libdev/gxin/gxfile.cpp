/*
 * G X F I L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gxin/gxfile.hpp"
#include <cstring>

//static
char GXFile::numTypeChar_[8]="C ULL D";

GXError GXFile::open(const std::string & filename)
{

  GXError result = FILE_OPEN_AGT;
  if (not isOpen()) {
     inFile_ = fopen(filename.c_str(),"rb");
     if(inFile_ != NULL)
     {
	    sectionCount_=0;
	    linePos_=0;
	    lineCount_=0;
   	    result = NO_ERROR_AGT;
       //  TODO:  reportAGTError(FILE_OPEN_AGT,(void*)fileName);
     }
  }

  return result;
}


bool GXFile::isOpen()
{
  bool result;
  return result = ((inFile_==NULL) ? false: true);
}

// Will be closed anyway by destructor
void GXFile::close()
{
  if (inFile_!=NULL)
  {
    fclose(inFile_);
    inFile_=NULL;
  }
}

GXError GXFile::skipHeader()
{
  GXError   err             = NO_ERROR_AGT;
  LabelAGT  type            = NOLABEL;
  char      header[]        = "AGFLI-T";
  char      localHeader[13] = {0x00};
  short     i               = 0;
  short     j               = 0;

  PRE(isOpen());

  err = readLine();
  if(err == NO_ERROR_AGT)
  {
    err = findTag(LABEL_TAG, true);
    if(err == NO_ERROR_AGT)
    {
      for(i = 0; i < 7; i++)
      {
        localHeader[i] = currentLine_[linePos_++];
      }
      if(strcmp(localHeader, header) != 0)
      {
        err = HEADER_NAME_AGT;
        err = reportAGTError(err,(void*)localHeader);
      }
      else
      {
        for(j = 0; j < 4; j++)
        {
          localHeader[i++] = currentLine_[linePos_++];  // read number
        }
        localHeader[i] = currentLine_[linePos_++];// read type of release
        if(charIsWhiteSpace(currentLine_[linePos_]) == false)
        {
          err = HEADER_VERSION_AGT;
          err = reportAGTError(err,NULL);
        }
      }
    }
  }

  return err;
}


GXError GXFile::skipSection(void)
{
  GXError err          = NO_ERROR_AGT;
  ULONG   secCount      = 1;
  char    currentChar  = 0x00;
  bool   over         = false;

  PRE(isOpen());

  while((currentChar != '{') && !(currentChar == EOF))
  {
    currentChar = getCharMoveToNext();
    // currentChar = currentLine[linePos++];
    if(currentChar == 0x00)
    {
      readLine();
      currentChar = getCharMoveToNext();
      // currentChar = currentLine[linePos++];
    };
  }

  if(currentChar == EOF)
  {
    currentChar = '{';
    err = TOKEN_AGT;
  }
  else
  {
    while(!over)
    {
      currentChar = getCharMoveToNext();
      // currentChar = currentLine[linePos++];
      if(currentChar == 0x00)
      {
        readLine();
        currentChar = getCharMoveToNext();
        // currentChar = currentLine[linePos++];
      };
      if(currentChar == '{')
        secCount++;
      if(currentChar == '}')
      {
        secCount--;
        if(secCount == 0)
          over = true;
      }
      if(currentChar == EOF)
      {
        currentChar = '}';
        err = TOKEN_AGT;
        over = true;
      }
    }
  }

  if(err != NO_ERROR_AGT)
  {
    err = reportAGTError(err,(void*)&currentChar);
  }

  return err;
}

GXFile::GXFile():
sectionCount_(0),
linePos_(0),
lineCount_(0),
inFile_(NULL)
{

    TEST_INVARIANT;
}

GXFile::~GXFile()
{
  TEST_INVARIANT;
  if (inFile_!=NULL)
  {
	fclose(inFile_);
    inFile_=NULL;
  }
}

void GXFile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GXFile& t )
{

    o << "GXIFile " << (void*)&t << " start" << std::endl;
    o << "GXIFile " << (void*)&t << " end" << std::endl;

    return o;
}

GXError GXFile::findTag(short which, bool report)
{
  GXError err      = NO_ERROR_AGT;
  char  validChar  = 0x00;


  switch(which)
  {
    case SECTION_START:
      validChar = '{';
      if(report)
        sectionCount_++;
      break;

    case SECTION_END:
      validChar = '}';
      if(report)
        sectionCount_--;
      break;

    case LABEL_TAG:
      validChar = '@';
      break;

    case AGID_TAG:
      validChar = '&';
      break;
  }

  err = getNextValidChar(validChar, report);

  return err;
}

GXError GXFile::readLine(void)
{
  GXError err;
  err  = NO_ERROR_AGT;
  void*   temp = NULL;

  PRE(isOpen());
  POST_DATA(int oldLineCount=lineCount_);

  lineCount_++;
  temp = fgets(currentLine_,LINESIZE,inFile_);
  if(temp == NULL)
  {

    if(sectionCount_ != 0)
    {
      err = LINE_AGT;
      err = reportAGTError(err,NULL);
    }
    else
      err = END_AGT;
  }
  linePos_ = 0;

  POST(lineCount_==oldLineCount+1);

  return err;
}

GXError GXFile::skipSpaces(void)
{
  GXError err;
  err = NO_ERROR_AGT;
  char    currentChar = '\0';

  PRE(isOpen());

  do
  {
    do  // comment extraction
    {
      currentChar = currentLine_[linePos_++];
      if(currentChar == 0x00)
      {
        readLine();
        currentChar = currentLine_[linePos_++];
      }
      if(currentChar == '/')        // should have a comment starting here
      {
        currentChar = currentLine_[linePos_];
        if(currentChar == '/')
        {
          err = readLine();         // comment, go to next line
          if(err != NO_ERROR_AGT)
          {
            return err;
          }
        }
        else
        {                           // only one comment character found error
          err = COMMENT_AGT;
          err = reportAGTError(err,(void*)"/");
          return err;
        }
      }
    } while((currentChar == '/'));
  } while(((currentChar == ' ') || (currentChar == 0x0A)) || (currentChar == 0x0D));

  linePos_--;  // cannot be 0 because someone would enter with -1

  return err;
}

char GXFile::getCharMoveToNext(void)
{
   PRE(isOpen());
   return currentLine_[linePos_++];
}

char GXFile::getChar(void)
{
   PRE(isOpen());
   return currentLine_[linePos_];
}

char GXFile::moveToNextGetChar(void)
{
   PRE(isOpen());
   return currentLine_[++linePos_];
}

void GXFile::moveToNext()
{
   PRE(isOpen());
   ++linePos_;
}


void GXFile::moveCursor(short amount)
{
  PRE(isOpen());
  linePos_+=amount;
}

GXError GXFile::readChar(char* num)
{
  GXError err    = NO_ERROR_AGT;
  ULONG   number = 0L;

  PRE(isOpen());
  PRE(num);

  err = readNumber(&number,CHAR_T);
  *num = (char)number;

  return err;

}

GXError GXFile::readUChar(UCHAR* num)
{
  GXError err    = NO_ERROR_AGT;
  ULONG   number = 0L;

  PRE(isOpen());
  PRE(num);

  err = readNumber(&number,CHAR_T);
  *num = (UCHAR)number;

  return err;

}

GXError GXFile::readShort(short* num)
{
  GXError err    = NO_ERROR_AGT;
  ULONG   number = 0L;

  PRE(isOpen());
  PRE(num);
  err = readNumber(&number,SHORT_T);
  *num = (short)number;

  return err;

}

GXError GXFile::readUShort(USHORT *num)
{
  GXError err    = NO_ERROR_AGT;
  ULONG   number = 0L;

  PRE(isOpen());
  PRE(num);
  err = readNumber(&number,USHORT_T);
  *num = (USHORT)number;

  return err;
}

GXError GXFile::readLong(long* num)
{
  GXError err    = NO_ERROR_AGT;
  ULONG   number = 0L;


  PRE(isOpen());
  PRE(num);

  err = readNumber(&number,LONG_T);

  *num = (long)number;
  return err;
}

GXError GXFile::readFloat(float* num)
{
  GXError err    = NO_ERROR_AGT;
  double  number = 0.0;

  PRE(isOpen());
  PRE(num);

  err = readFloatNum(&number, FLOAT_T);
  *num = (float)number;

  return err;
}

GXError GXFile::readFloatNum(double* num, const GXNumTypes& type)
{
  GXError err        = NO_ERROR_AGT;
  char    numStr[50] = {0x00}; //-XXXXX.XXXX or -X.XXXXE-XXX
  UCHAR   dotCount   = 0;
  UCHAR   expCount   = 0;
  short   i          = 0;
  bool   over       = false;

  PRE(isOpen());
  PRE(num);

  skipSpaces();

  do
  {
    numStr[i] = getCharMoveToNext();
    if(validNumberDigit(numStr[i], numStr[(i>0)?(i-1):(i)], i, type))
    {
      if(numStr[i] == '.')
      {
        dotCount++;
        if(dotCount > 1)
        {
          err = FLOAT_AGT;
          err = reportAGTError(err,(void*)numStr);
        }
      }
      if((numStr[i] == 'e') || (numStr[i] == 'E'))
      {
        expCount++;
        if(expCount > 1)
        {
          err = FLOAT_AGT;
          err = reportAGTError(err,(void*)numStr);
        }
      }
      if(type == FLOAT_T)
      {
        if(charIsWhiteSpace(getChar()))
          over = true;
        else
         if(currentLine_[linePos_] == '}')
          over = true;
      }
      else
      {
        if(currentLine_[linePos_] == ':')
          over = true;
      }
    }
    else // not valid digit
    {
      err = COUNT_AGT;
      err = reportAGTError(err,(void*)&currentLine_[linePos_-1]);
      return err;
    }
    i++;
  }while(!over);

  if(type != FLOAT_T)
  {
    if(currentLine_[++linePos_] != numTypeChar_[type])
    {
      err = TOKEN_AGT;
      err = reportAGTError(err,(void*)&type);
    }
    linePos_++; // put cursor in white space

  }
  *num = atof(numStr);// convert string to float

  return err;
}

GXError GXFile::readName(GXName &name, bool report)
{
  GXError   err            = NO_ERROR_AGT;
  char      localName[513] = {0x00};
  long      i              = 0L;
  bool      over           = false;
  char      endCharacter   = '}';

  skipSpaces();

  err = findTag(SECTION_START, false);
  if(err == NO_ERROR_AGT)
  {
    while(((getChar() != endCharacter) && (i < 512)) && (err == NO_ERROR_AGT))
    {
      if((getChar() != 0x0A) && (getChar() != 0xD))
      {
        localName[i++] = getCharMoveToNext();
      }
      else
      {
        err = NT_NAME_AGT;
        err = reportAGTError(err,NULL);
      }
    }
    if(i >= 512)
    {
      err = STR_LENGTH_AGT;
      err = reportAGTError(err, NULL);
    }
    if(err == NO_ERROR_AGT)
    {
      err = findTag(SECTION_END, false);
      if(err == NO_ERROR_AGT)
      {
         name=localName;
      }
      else  // couldnt find the tag
      {
        err = NT_NAME_AGT;
        err = reportAGTError(err,NULL);
      }
    }
  }
  else
  {
    if(report)
    {
      err = INV_NAME_AGT;
      err = reportAGTError(err,NULL);
    }
  }

  return err;
}

GXError GXFile::readNumber(ULONG *number, const GXNumTypes& type)
{
  GXError   err;
  err = NO_ERROR_AGT;
  char     numStr[12]  = {0x00};
  short    i           = 0;
  bool     over        = false;
  char     lastChar    = ':';

  PRE(isOpen());
  PRE(number);

  skipSpaces();

  if(type == SHORT_T) // shorts don't have a type specifier -> ':S'
    lastChar = '9';

  do
  {
    numStr[i]=currentLine_[linePos_++];
    if(validNumberDigit(numStr[i],numStr[(i > 0)?(i - 1):(i)],i,type))
    {
      if(type == SHORT_T)
      {
        if(charIsWhiteSpace(currentLine_[linePos_]))
          over = true;
        else
          if(currentLine_[linePos_] == '}')
            over = true;

      }
      else
      {
        if(currentLine_[linePos_] == ':')
          over = true;
      }
    }
    else
    {
      err = COUNT_AGT;
      err = reportAGTError(err,(void*)&currentLine_[linePos_ - 1]);
      return err;
    }

    i++;
  }while(!over);


  if(type != SHORT_T)
  {
    if(currentLine_[++linePos_] != numTypeChar_[type])
    {
      err = TOKEN_AGT;
      err = reportAGTError(err,(void*)&(numTypeChar_[type]));
    }
  }

  if(err != FAIL_AGT)
  {
    linePos_++;  // put the cursor in the white space
    if(type == ULONG_T)
      linePos_++; // skip the U
    *number = atol(numStr);
  }
  return err;

}

GXError GXFile::reportAGTError(GXError error, void *info)
{
// TODO: find a way to remove this ugly void* argument passing and have
// method accept different type of argument according to the error type

  // print error message
  printf("Error at line %i:%i ",lineCount_,linePos_+1);
  switch(error)
  {
    case FILE_OPEN_AGT:
      printf("couldn't open the file ");
      printf("%s",(char*)info);
      break;

    case HEADER_NAME_AGT:
      printf("name %s is an invalid header",(char*)info);
      break;

    case HEADER_VERSION_AGT:
      printf("unsupported version of AGT file");
      break;

    case LABEL_AGT:
// TODO, make sure the unsupported label is output, as in the AGT APIs
//      printf("the label @%s cannot be used in this section",labelList[*((LabelAGT*)info)]);
      printf("this label cannot be used in this section");
      break;

    case MEMORY_AGT:
      printf("couldn't allocate memory for");
      printf("%s",(char*)info);
      break;

    case LINE_AGT:
      printf("couldn't read the next line. EOF maybe present in the line");
      break;

    case COMMENT_AGT:
      printf("invalid comment. Expecting another /");
      break;

    case TOKEN_AGT:
      printf("unexpected token. Expecting %c",*((char*)info));
      break;

    case FLOAT_AGT:
      printf("invalid float number %s",(char*)info);
      break;

    case COUNT_AGT:
      printf
      (
        "unexpected token. Expecting number digit or :, but received %c"
        ,*((char*)info)
      );
      break;

    case DUP_AGID_AGT:
      {
//        AGIDPos* agidPos = NULL;

//        agidPos = (AGIDPos*)info;
//        printf("duplicate AGID &%08X", agidPos->agid);
//        if(agidPos->pos != 0)
//         printf(":%d", agidPos->pos);
		  printf("duplicate GXIDPos");

      }
      break;

    case INV_AGID_AGT:
      printf("invalid AGID &%s",(char*)info);
      break;

    case LABEL_TOOLONG_AGT:
      printf("label @%s too long. Probably invalid",(char*)info);
      break;

    case MATRIX_ROW_AGT:
      printf("cannot extract transform matrix and translate vector properly");
      break;

    case STR_LENGTH_AGT:
      printf("name too long. Limit to 512 characters");
      break;

    case NT_NAME_AGT:
      printf("name no terminated by }");
      break;

    case INV_NAME_AGT:
      printf("invalid name - names must be enclosed in braces");
      break;
  }
  printf(".\n");
  return FAIL_AGT;
}

bool GXFile::charIsWhiteSpace( char character )
{
  bool answer = false;


  switch(character)
  {
                // TODO add '/' to be a white space ?
    case 0x00:
    case 0x0A:
    case 0x0D:
    case 0x20:
      answer = true;
    default:
      break;
  }

  return answer;

}

bool GXFile::validNumberDigit(char digit, char prev, short position, GXNumTypes type)
{
  bool valid = false;


  if((digit >= '0') && (digit <= '9'))
  {
    valid = true;
  }
  else
  {
    switch(digit)
    {
      case '-':
      case '+':
        if(position == 0)
        {
            if((type != USHORT_T) && (type != ULONG_T))
              valid = true;
        }
        else
        {
          if((type == FLOAT_T) || (type == DOUBLE_T))
            if((prev == 'e') || (prev == 'E'))
              valid = true;
        }
        break;

      case '.':
        if((type == FLOAT_T) || (type == DOUBLE_T))
          valid = true;
        break;

      case 'e':
      case 'E':
        if((type == FLOAT_T) || (type == DOUBLE_T))
          valid = true;
        break;

      default:
        break;
    }
  }
  return valid;
}

GXError GXFile::getNextValidChar(char validChar, bool report)
{
  GXError err = NO_ERROR_AGT;


  err = skipSpaces();
  if(err == NO_ERROR_AGT)
  {
    if(currentLine_[linePos_] != validChar)
    {
      if((sectionCount_ == 0) && (lineCount_ > 1))
      {
        err = END_AGT;
        report = false;
      }
      else
        err = TOKEN_AGT;
      if(report)
      {
        // TODO why this ???
        if((currentLine_[linePos_] != '}') || (validChar != '@'))
            err = reportAGTError(err,(void*)&validChar);
      }
    }
    else  linePos_++;  //position cursor after the char
  }
  return err;
}

bool GXFile::validHexDigit(char digit)
{
  bool isValid = false;


  digit = (char)toupper((int)digit);

  if( (digit >= '0') && ( digit <= '9'))
    isValid = true;

  if( (digit >= 'A') && ( digit <= 'F') )
    isValid = true;

  return isValid;
}

GXError GXFile::readGXIdPos(GXIdPos &gxidpos, bool report, bool validate, void* pointer)
{
  GXError err;
  err = NO_ERROR_AGT;
  char    agidStr[9]  = {'\0'};
  char    posStr[5]   = {'\0'};
  short   i           = 0;
  ULONG   pos         = 0L;

  PRE(isOpen());

  err = findTag(AGID_TAG, report);
  if(err == NO_ERROR_AGT)
  {
   // linePos++; // skip tag
    for(i = 0; (i < 8) && (err == NO_ERROR_AGT); i++)
    {
	  agidStr[i] = getCharMoveToNext();
      // agidStr[i] = currentLine[linePos++];

      if(validHexDigit(agidStr[i]) == false)
      {
        err = INV_AGID_AGT;
        reportAGTError(err,(void*)agidStr);
		// err = ReportAGTError(err,(void*)agidstr);
      }
    }

    if(err == NO_ERROR_AGT)
    {
	  gxidpos.gxid(strtoul(agidStr,NULL,16));
      // agidPos->agid = strtoul(agidStr,NULL,16);

      if(getChar() == ':')
      // if(currentLine[linePos] == ':')
      {
        // linePos++;
        moveToNext();
        // err = ReadNumber(&pos, SHORT_T);
		err = readNumber(&pos, SHORT_T);
      }
      // agidPos->pos = (short)pos;
      gxidpos.pos((short)pos);

      // TODO: implement the AGID hash table:
      // if((validate) && (err == NO_ERROR_AGT))
        // err = RegisterAGID(agidPos, pointer, FALSE);
		// in them meantime the following prevents
		// compilation warnings
		bool validatewarning=validate;
		void* pointerwarning=pointer;
    }
  }
  return err;
}

GXError GXFile::readLabel(GXLabel &label, bool report)
{
  GXError err         = NO_ERROR_AGT;
  short   i           = 0;

  PRE(isOpen());

  label.init();

  err = findTag(LABEL_TAG, report);
  if(err == NO_ERROR_AGT)
  {
    // while(charIsWhiteSpace(currentLine[linePos]) == FALSE)
    while(charIsWhiteSpace(getChar()) == false)
    {
      label.nameAppend(getCharMoveToNext());
      // label[i++] = currentLine[linePos++];  // ++linePos to skip tag
      if(i >= LABELLENGTH)
      {
        currentLine_[linePos_] = 0x00;
        err = LABEL_TOOLONG_AGT;
        if(report)
//		TODO: pass the label as an argument to the report
// 		error report function
          reportAGTError(err,NULL);
      }
    }

    if(err == NO_ERROR_AGT)
    {
      label.findLabelType(); // returns the type of label
    }
  }
  return err;
}

GXError GXFile::readColor(GXColor& color)
{
  GXError   err = NO_ERROR_AGT;
  UCHAR gxcolorcmp;

  PRE(isOpen());

  err = findTag(SECTION_START, true); // find opening braces
  if(err == NO_ERROR_AGT)
  {
    err = readUChar(&gxcolorcmp);
    if(err == NO_ERROR_AGT)
    {
	  color.red(gxcolorcmp);
      err = readUChar(&gxcolorcmp);
      if(err == NO_ERROR_AGT)
      {
	    color.green(gxcolorcmp);
        err = readUChar(&gxcolorcmp);
        if(err == NO_ERROR_AGT)
        {
		  color.blue(gxcolorcmp);
          err = findTag(SECTION_END, true); // read close section tag
        }
      }
    }
  }
  return err;
}

GXError GXFile::readUVCoords(GXUVCoords &uv)
{
  GXError   err = NO_ERROR_AGT;
  float coord;

  PRE(isOpen());

  err = findTag(SECTION_START, false);  // look for tag but dont report or count it
  if(err == NO_ERROR_AGT)                   // did find one, must be uv coord
  {
    moveCursor(-1);
    findTag(SECTION_START, true);       // no need to check for error,
                                        // we already found the tag before
                                        // now we make it count

    err = readFloat(&coord);            // read u value
    if(err == NO_ERROR_AGT)
    {
	  uv.u(coord);
      err = readFloat(&coord);          // read v value
      if(err == NO_ERROR_AGT)
      {
	    uv.v(coord);
        err = findTag(SECTION_END, true);
      }
    }
  }
  // else
  //   err = UV_ABSENT;


  return err;
}

GXError GXFile::readMesh(GXMesh& mesh, const GXLabel& meshtype)
{
  GXError      err = NO_ERROR_AGT;
  GXIdPos      agid;
//  QLLMeshAG*   temp = NULL;


    PRE(isOpen());

    err = readGXIdPos(agid, true, true, NULL); // read AGID
//    err = readGXIdPos(agid, true, true, (void*)temp); // read AGID
    if(err == NO_ERROR_AGT)
    {
	  mesh.id(agid);
      err = findTag(SECTION_START, true);           // find opening braces
      if(err == NO_ERROR_AGT)
      {
        switch(meshtype.type())
        {
          case TRIMESH:
            err = readTriMesh(mesh);
            break;

          case QUADMESH:
            err = readQuadMesh(mesh);
            break;

          default:
            err = skipSection();
            break;
        }
        if(err != FAIL_AGT)
          err = findTag(SECTION_END, true);           // find closing braces
      }
    }

// TODO: understand whether this has sthg to
// do with hierarchy building
//    if(err == NO_ERROR_AGT)
//    {
//      err = AddMeshToList(temp, agid);
//    }
//    else
//    {
//      if(temp)
//        FREE(temp); // TODO FreeMeshAG();
//      temp = NULL;
//    }
//  }

  return err;

}

GXError GXFile::readTriMesh(GXMesh& mesh)
{
  GXError      err       = NO_ERROR_AGT;
  GXLabel      label;
  UCHAR        numVertex = 3;


  PRE(isOpen());

  do
  {
    err = readLabel(label, true);// read next label
    if(err == NO_ERROR_AGT)
    {
      switch(label.type())
      {
        case POINTARRAY:
          err = read3DPointArray(mesh);
          break;

        case TRIANGLEARRAY:
          err = readPolygonArray(mesh, numVertex);
          break;

        case NORMALARRAY:
         err = readNormalArray(mesh);
          break;

        default:
          err = skipSection();
          //err = LABEL;
          //err = ReportAGTError(err,(void*)&type);
          break;
      }// switch
    }
  }while(err == NO_ERROR_AGT);

  return err;
}

GXError GXFile::readQuadMesh(GXMesh& mesh)
{
  GXError      err  = NO_ERROR_AGT;
  GXLabel      label;
  UCHAR        numVertex;


  PRE(isOpen());

  do
  {
    err = readLabel(label, true);// read next label
    if(err == NO_ERROR_AGT)
    {
      switch(label.type())
      {
        case POINTARRAY:
          err = read3DPointArray(mesh);
          break;

        case TRIANGLEARRAY:
          numVertex = 3;
//          err = ReadPolygonArray(mesh, numVertex);
			err = skipSection();
          break;

        case QUADRANGLEARRAY:
          numVertex = 4;
//          err = ReadPolygonArray(mesh, numVertex);
			err = skipSection();
          break;

        case NORMALARRAY:
//          err = ReadNormalArray(mesh);
			err = skipSection();
          break;

        default:
          err = skipSection();
          //err = LABEL;
          //err = ReportAGTError(err,(void*)&type);
          break;
      }// switch
    }
  }while(err == NO_ERROR_AGT);

  return err;

}

GXError GXFile::read3DPointArray(GXMesh& mesh)
{
  GXError       err   = NO_ERROR_AGT;
  GXIdPos       agid;
  long          i     = 0L;
  long          count = 0L;


  PRE(isOpen());

  err = readGXIdPos(agid, true, true, NULL); // read AGID
  if(err == NO_ERROR_AGT)
  {
    err =  readLong(&count);                  // read count
    if(err == NO_ERROR_AGT)
    {
      err = findTag(SECTION_START, true);      // find opening braces
      if(err == NO_ERROR_AGT)
      {
		// performn the allocation of points array
        mesh.numPoints((USHORT)count);
        for(i = 0;(i < count) && (err == NO_ERROR_AGT); i++)
        {
		  GXPoint3 gxpoint;
          err = read3DPoint(gxpoint);  // Read 3D point
		  mesh.addPoint(gxpoint);
		  // TODO: replace all this by
		  // err  = read3DPoint(mesh.point(i));
		  // as soon as vector::resize is implemented and
		  // GXMesh::numPoint(int newNumpoint) calls it.
        }

        if(err == NO_ERROR_AGT)
        {
          err = findTag(SECTION_END, true);        // find closing braces
        }
      }
    }
  }
  return err;
}

GXError GXFile::read3DPoint(GXPoint3& point)
{
  GXError       err = NO_ERROR_AGT;

  PRE(isOpen());

  err = findTag(SECTION_START, true); // find opening braces
  if(err == NO_ERROR_AGT)
  {
	float temp;
    err = readFloat(&temp);
    if(err == NO_ERROR_AGT)
    {
      point.x(temp);
      err = readFloat(&temp);
      if(err == NO_ERROR_AGT)
      {
	    point.y(temp);
        err = readFloat(&temp);
        if(err == NO_ERROR_AGT)
        {
		  point.z(temp);
          err = findTag(SECTION_END, true); // read close section tag
        }
      }
    }
  }
  return err;
}

GXError GXFile::readNormalArray(GXMesh& mesh)
{
  GXError        err    = NO_ERROR_AGT;
  long           count  = 0L;
  long           i      = 0L;
  GXIdPos        agid;
  GXVect3 		 gxnormal;

  PRE(isOpen());

  err = readGXIdPos(agid, true, true, NULL);        // read AGID
  if(err == NO_ERROR_AGT)
  {
    err = readLong(&count);            // read count
    if(err == NO_ERROR_AGT)
    {
      err = findTag(SECTION_START, true); // find opening braces
      if(err == NO_ERROR_AGT)
      {
        for(i = 0; (i < count) && (err == NO_ERROR_AGT); i++)
        {
		   // used to be a list management here...
           err = readVector3(gxnormal);// Read 3D point
		   mesh.addNormal(gxnormal);
		  // TODO: replace all this by
		  // err  = readVector3(mesh.normal(i));
		  // as soon as vector::resize is implemented and
		  // GXMesh::numNormals(int newNumNormals) calls it.
        } // for()
        if(err == NO_ERROR_AGT)
          err = findTag(SECTION_END, true);// find closing braces
      }
    }
  }
  if(err != NO_ERROR_AGT)
    err = FAIL_AGT;

  return err;
}

GXError GXFile::readVector3(GXVect3& vector)
{
  GXError       err = NO_ERROR_AGT;


  PRE(isOpen());

  err = findTag(SECTION_START, true); // find opening braces
  if(err == NO_ERROR_AGT)
  {
	float temp;
    err = readFloat(&temp);
    if(err == NO_ERROR_AGT)
    {
	  vector.x(temp);
      err = readFloat(&temp);
      if(err == NO_ERROR_AGT)
      {
	    vector.y(temp);
        err = readFloat(&temp);
        if(err == NO_ERROR_AGT)
        {
		  vector.z(temp);
          err = findTag(SECTION_END, true);  // find opening braces
        }
      }
    }
  }
  return err;
}

GXError GXFile::readVector4(GXVect4& vector)
{
  GXError err = NO_ERROR_AGT;
  float val;

  err = findTag(SECTION_START, true);         // read open section tag
  if(err == NO_ERROR_AGT)
  {
    err = readFloat(&val);
    vector.x(val);              // read float
    if(err == NO_ERROR_AGT)
    {
      err = readFloat(&val);
      vector.y(val);            // read float
      if(err == NO_ERROR_AGT)
      {
        err = readFloat(&val);
        vector.z(val);          // read float
        if(err == NO_ERROR_AGT)
        {
          err = readFloat(&val);
          vector.h(val);       // read float
          if(err == NO_ERROR_AGT)
          {
            err = findTag(SECTION_END, true); // read close section tag
          }
        }
      }
    }
  }

  return err;
}

GXError GXFile::readPolygonArray(GXMesh &mesh, UCHAR numVertex)
{
  GXError       err   = NO_ERROR_AGT;
  GXLabel      type;
  long          count = 0L;
  long          i     = 0L;
  GXIdPos       agid;

  PRE(isOpen());

  err = readGXIdPos(agid, true, true, NULL);        // read AGID
  if(err == NO_ERROR_AGT)
  {
    err = findTag(SECTION_START, true); // find opening braces
    if(err == NO_ERROR_AGT)
    {
      err = readLabel(type, true);       // read polygon tag @Polygon
      if((err != NO_ERROR_AGT) || (type.type() != POLYGON))
      {
        if(type.type() != POLYGON)
        {
          err = LABEL_AGT;
          err = reportAGTError(err,(void*)&type);
        }

      }
    }
  }
  if(err == NO_ERROR_AGT)
  {
    err = readGXIdPos(agid, true, true, NULL);        // read AGID
    if(err == NO_ERROR_AGT)
    {
      err = readLong(&count);      // read count
      if(err == NO_ERROR_AGT)
      {
        err = findTag(SECTION_START, true); // find opening braces
        if(err == NO_ERROR_AGT)
        {
		  // TODO: replace all this by...
		  // err = readPolygon(mesh.polygon(i));
		  // once numPolygons(int) implements the resize
		  // method for vectors.

		  // This initialises the polygons arrays
		  // for the push_back (addPolygon) routine
          mesh.numPolygons(count);
          for(i = 0; (i < count) && (err == NO_ERROR_AGT); i++)
          {
		    GXPolygon3  gxpolygon;
            err = readPolygon(gxpolygon, numVertex);// read polygon
			if (err==NO_ERROR_AGT) {
			  mesh.addPolygon(gxpolygon);
			}
          }
          if(err == NO_ERROR_AGT)
          {
            err = findTag(SECTION_END, true);    // find closing braces
            if(err == NO_ERROR_AGT)
            {
              err = findTag(SECTION_END, true);// find closing braces
            } //
          } // loop
        }     // find opening braces
      }       // read long
    }         // read agid
  }
  return err;
}

GXError GXFile::readPolygon(GXPolygon3 &polygon, UCHAR numVertex)
{
  GXError      err   = NO_ERROR_AGT;
  GXLabel      type;
  GXIdPos      agid;
  long         count = 0L;
  long         i     = 0L;
  short       gxtransparency;
  UCHAR 	  gxflag;
  GXMat		  gxmat;

  PRE(isOpen());

  err = findTag(SECTION_START, true); // find opening braces
  if(err == NO_ERROR_AGT)
  {
    err = readLabel(type, true);  // read @PolygonVertex
    if((err != NO_ERROR_AGT) || (type.type() != POLYGONVERTEX))
    {
      if(type.type() != POLYGONVERTEX)
      {
        err = LABEL_AGT;
        err = reportAGTError(err,(void*)&type);
      }
      return err;
    }
  }

  if(err == NO_ERROR_AGT)
  {
    err = readGXIdPos(agid, true, true, NULL);        // read AGID
    if(err == NO_ERROR_AGT)
    {
      err = readLong(&count);      // read count
      if(err == NO_ERROR_AGT)
      {
        if(count != numVertex)
        {
          err = TOKEN_AGT;
          err = reportAGTError(err,NULL);
        }
        else
        {
          err = findTag(SECTION_START, true); // find opening braces
        }
      }
    }
  }


  if(err == NO_ERROR_AGT)
  {
    polygon.numVertices((USHORT)count);
    for(i = 0; (i < count) & (err == NO_ERROR_AGT); i++)
    {
      GXPolyVert3  gxvertex;
      err = readPolygonVertex(gxvertex);
	  polygon.addVertex(gxvertex);
      // read vertex
    }
	// We don't set the color status now, since it could be overwrited
	// later when the material is set
    if(err == NO_ERROR_AGT)
    {
     err = findTag(SECTION_END, true);// read close section tag
      if(err == NO_ERROR_AGT)
      {
        err = readShort(&gxtransparency);// read transparency
        gxmat.transparancy(gxtransparency);
        if(err == NO_ERROR_AGT)
        {
          err = readUChar(&gxflag);// read flag
          if(err == NO_ERROR_AGT)
          {
		    polygon.flag(gxflag);
            err = readGXIdPos(agid, true, false, NULL);// read texture AGID
            if(err == NO_ERROR_AGT)
            {
			  gxmat.textureId(agid);
              polygon.mat(gxmat);
			  // set the color status after the material has been set
			  polygon.setColorStatus();
              err = findTag(SECTION_END, true);// read close section tag
            }
          }
        }
      }
    }
  }


  return err;
}

GXError GXFile::readPolygonVertex(GXPolyVert3& vertex)
{
  GXError       err         = NO_ERROR_AGT;
  long          normalIndex = 0L;
  long          vertexIndex = 0L;
  void*         info        = NULL;
  GXColor		gxcolor;
  GXUVCoords	gxuv;

  PRE(isOpen());

  err = findTag(SECTION_START, true); // find opening braces
  if(err == NO_ERROR_AGT)
  {
    err = readLong(&vertexIndex);// read vertex index
    if(err == NO_ERROR_AGT)
    {
      vertex.pointIndex((USHORT)vertexIndex);

      err = readLong(&normalIndex);
      if(err == NO_ERROR_AGT)
      {
        vertex.normalIndex((USHORT)normalIndex);

        err = readColor(gxcolor);// read color
        if(err == NO_ERROR_AGT)
        {
		  vertex.color(gxcolor);
          readUVCoords(gxuv);// read uvcoord // dont check for error it is optional
          if((err == UV_ABSENT_AGT) || (err == NO_ERROR_AGT))
          {
		    vertex.uv(gxuv);
            err = findTag(SECTION_END, true);// read close section tag
          }
        }
      }
    }
  }
  return err;
}

GXError GXFile::readTextures(GXTextures& textures)
{
  GXError   err   = NO_ERROR_AGT;
  GXIdPos   agid;
  GXTexture texture;

  PRE(isOpen());

  err = readGXIdPos(agid, true, true, NULL);// read agid
  if(err == NO_ERROR_AGT)
  {
    err = findTag(SECTION_START, true); // read open section tag
    if(err == NO_ERROR_AGT)
    {
      while(err == NO_ERROR_AGT)
      {
        err = readBitmapTexture(texture);  // read label
		if (err == NO_ERROR_AGT)
		{
		  if (textures.find(texture.id())!=textures.end())
		  {
		    reportAGTError(DUP_AGID_AGT, NULL);
		  }
		  else
		  {
		    GXTextures::value_type new_val(texture.id(),texture);
		    textures.insert(new_val);
		  }
		} // if (err==NO_ERROR_AGT)
      }	// while (err==NO_ERROR_AGT)
      if(err == TEXTURE_END_AGT)
        err = findTag(SECTION_END, true); // read close section tag
    } // if (err==NO_ERROR_AGT)
  }
  return err;
}

GXError GXFile::readBitmapTexture(GXTexture &texture)
{
  GXError       err     = NO_ERROR_AGT;
  GXIdPos       agid;
  GXLabel      type;
  GXName       gxname;

  err = findTag(SECTION_END, false);          // find tag no report
  moveCursor(-1);                             // move back one space
  if(err == NO_ERROR_AGT)                         // if we didn't find one
  {
    err = TEXTURE_END_AGT;  // say that we found the end of the textures
  }
  else
  {
    err = readLabel(type, true);             // read label
    if(err == NO_ERROR_AGT)
    {
//    TODO?: err = readGXIdPos(agid, true, true, (void*)texture);   // read agid
      err = readGXIdPos(agid, true, true, NULL);   // read agid
      texture.id(agid);
      if(err == NO_ERROR_AGT)
      {
        err = findTag(SECTION_START, true);   // read open section tag
        if(err == NO_ERROR_AGT)
        {
          err = readName(gxname, true);
          texture.name(gxname); // read name
          if(err == NO_ERROR_AGT)
          {
            err = findTag(SECTION_END, true); // read close section tag
          }
        }
      }
    }
  }

  return err;
}

GXError GXFile::readHierarchy(GXHier& hierarchy)
{
  GXError       err  = NO_ERROR_AGT;
  GXIdPos       agid;
  GXLabel      type;


  err = readGXIdPos(agid, true, true, NULL);
  if(err == NO_ERROR_AGT)
  {
    err = findTag(SECTION_START, true);
    if(err == NO_ERROR_AGT)
    {
      err = readLabel(type, true);
      if((err == NO_ERROR_AGT) && (type.type() == CHILD))
      {
        err = readChild(hierarchy);    // hierarchy is parent node the
                                              // meshList is hierarchy->child
      }
      if(err == NO_ERROR_AGT)
      {
        err = findTag(SECTION_END, true);
      }
    }
  }
  return err;
}

GXError GXFile::readChild(GXHier& mesh)
{
  GXError   err   = NO_ERROR_AGT;
  long      count = 0L;
  GXIdPos   agid;
  GXLabel   type;
  short     i     = 0;

  PRE(isOpen());

  err = readGXIdPos(agid, true, true, NULL);          // read agid
  if(err == NO_ERROR_AGT)
  {
    err = readLong(&count);               // read count
    if(err == NO_ERROR_AGT)
    {
      err = findTag(SECTION_START, true );  // read open section tag
      if(err == NO_ERROR_AGT)
      {
        // allocate memory for children
		mesh.createChildren(count);
        for(i = 0; (i < count) && (err == NO_ERROR_AGT); i++)  // for children count
        {
          agid.pos(i);
          err = readChildInfo(mesh.child(i), agid);// read childinfo

        }
        if(err == NO_ERROR_AGT)
        {
          err = findTag(SECTION_END, true);// read close section tag
        }
      }
    }
  }
  return err;
}

GXError GXFile::readChildInfo(GXHier& mesh, GXIdPos agidPos)
{
  GXError       err  = NO_ERROR_AGT;
  GXLabel       type;
  GXName        gxmeshName;
  GXIdPos       gxmeshId;
  GXIdPos		zeroId;

  PRE(isOpen());

  mesh.parentId(agidPos);

  if(err == NO_ERROR_AGT)
  {
    err = findTag(SECTION_START, true);
    if(err == NO_ERROR_AGT)
    {
      err = readName(gxmeshName, true);
      mesh.meshName(gxmeshName);              // read name (opt)
      if(err == NO_ERROR_AGT)
      {
        err = readGXIdPos(gxmeshId, false, false, NULL); // read AGID (opt)
		if (gxmeshId==zeroId)
		{
		  // the node has no mesh attached to it
		  mesh.meshId(zeroId);
		}
		else
		{
		  mesh.meshId(gxmeshId);
        }

        if((err == NO_ERROR_AGT) || (err == TOKEN_AGT))
        {
		  // TODO, this UCHAR appears in the new file format
		  // find out what it means and implement it
		  UCHAR dummy;
		  err = readUChar(&dummy);

          if((err == NO_ERROR_AGT))
          {
            err = NO_ERROR_AGT;
            while(err == NO_ERROR_AGT) // TODO ????
            {
              err = readLabel(type, false);
              if(err == NO_ERROR_AGT)
              {
                switch(type.type())
                {
                  case TRANSFORM:
                    err = readTransform(mesh);
                    break;

                  case CHILD:
                    err = readChild(mesh);
                    break;

                  default:
                    err = LABEL_AGT;
// TODO                  reportAGTError(err,(void*)type);
                         reportAGTError(err,NULL);
                    break;
                }
              }
            }
		  }
          if(err == TOKEN_AGT)
          {
              err = findTag(SECTION_END, true);
          }
        }
      }
    }
  }

  return err;
}


GXError GXFile::readTransform(GXHier& mesh)
{
  GXError       err = NO_ERROR_AGT;
  GXLabel      type;
  GXIdPos       agid;

  PRE(isOpen());

  err = readGXIdPos(agid, true, true, NULL);    // read AGID
  if(err == NO_ERROR_AGT)
  {
    err = findTag(SECTION_START, true);         // read open section tag
    if(err == NO_ERROR_AGT)
    {
      err = readLabel(type, true);  // read label
      if(err == NO_ERROR_AGT)
      {
        err = readGXIdPos(agid, true, true, NULL);    // read AGID
        if(err == NO_ERROR_AGT)
        {
          switch(type.type())
          {
            case MATRIX4X4:
              err = readTransformFromMatrix4X4(mesh);
              break;

            case MATRIX3X3:
              err = readTransformFromMatrix3X3(mesh);
              break;

            case COMPONENTTRANSFORM:
              err = readComponentTransform(mesh);
              break;

            default:
              err = LABEL_AGT;
              reportAGTError(err,(void*)&type);
              break;
          }
          if(err == NO_ERROR_AGT)
          {
            err = findTag(SECTION_END, true);// read close section tag
          }
        }
      }
    }
  }
  return err;
}

GXError GXFile::readTransformFromMatrix4X4(GXHier& mesh)
{
  GXError      err = NO_ERROR_AGT;
  GXVect4     vector[4];
  short         i = 0;
  GXMatrix3 gxtransform;
  GXVect3 gxtranslate;


  err = findTag(SECTION_START, true);  // read open section tag
  for(i = 0; (i < 4) && (err == NO_ERROR_AGT); i++)
  {
    err = readVector4(vector[i]);
  }

  if(err == NO_ERROR_AGT)
  {
    if( ((vector[3].x() != 0.0f) || (vector[3].y() != 0.0f)) ||
        ((vector[3].z() != 0.0f) || (vector[3].h() != 1.0f)) )
    {
      // Warn user that the matrix is not really valid
      reportAGTError(MATRIX_ROW_AGT, NULL);
    }

    // convert values
    gxtransform.xx(vector[0].x());
    gxtransform.xy(vector[0].y());
    gxtransform.xz(vector[0].z());

    gxtransform.yx(vector[1].x());
    gxtransform.yy(vector[1].y());
    gxtransform.yz(vector[1].z());

    gxtransform.zx(vector[2].x());
    gxtransform.zy(vector[2].y());
    gxtransform.zz(vector[2].z());

    gxtranslate.x(vector[0].h());
    gxtranslate.y(vector[1].h());
    gxtranslate.z(vector[2].h());

	mesh.transform(gxtransform);
	mesh.translate(gxtranslate);

    err = findTag(SECTION_END, true);// read close section tag

  }

  return err;
}

GXError GXFile::readTransformFromMatrix3X3(GXHier& mesh)
{
  GXError       err = NO_ERROR_AGT;
  GXVect3       gxvect;
  GXVect3       gxtranslate;
  GXMatrix3		gxtransform;

  err = findTag(SECTION_START, true);  // read open section tag
  if(err == NO_ERROR_AGT)
  {
    err = readVector3(gxvect);
	gxtransform.x(gxvect);
    if(err == NO_ERROR_AGT)
    {
      err = readVector3(gxvect);
	  gxtransform.y(gxvect);
      if(err == NO_ERROR_AGT)
      {
        err = readVector3(gxvect);
		gxtransform.z(gxvect);
        if(err == NO_ERROR_AGT)
        {
          err = readVector3(gxtranslate);
          if(err == NO_ERROR_AGT)
          {
            err = findTag(SECTION_END, true);// read close section tag
			mesh.transform(gxtransform);
  			mesh.translate(gxtranslate);
          }
        }
      }
    }
  }

  return err;
}

GXError GXFile::readComponentTransform(GXHier& mesh)
{
  GXError       err = NO_ERROR_AGT;

  // prevent compiler warning..
  mesh.allChildrenSupplied();
  // TODO not supported yet
  LIONEL_STREAM("readComponentTransform, not supported yet");
  return err;
}
/* End GXFILE.CPP **************************************************/
