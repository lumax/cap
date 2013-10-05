/*
Bastian Ruppert
*/
//read directories
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <termios.h>

#include <sys/stat.h>
#include <unistd.h>

#include <string.h>

#include <errno.h>

#include <SDL/SDL_ttf.h>
#include "LL.h"
#include "Event.h"
#include "Tools.h"
#include "Globals.h"
#include "Button.h"
#include "Poll.h"
#include "Label.h"
#include "Screen.h"
#include "Main.h"

#include "v4l_capture.h"
#include "Rezept.h"
#include "Protocol.h"
#include "pec_cmd.h"
#include "MBProt.h"

#include "MenuBar.h"
#include "ArbeitsDialog.h"
#include "ErrorDialog.h"
#include "Dialoge.h"
#include "BackupDialog.h"

namespace EuMax01
{


  void BackupDialog::escape_listener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    ad->Parent->showBackupMenuDialog();
  }

  void BackupDialog::backup_listener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    ad->Parent->showBackupDialog();
    printf("BackupDialog backup_listener\n");
  }

  void BackupDialog::filter_listener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    ad->Parent->showBackupDialog();
  }

  void BackupDialog::pgup_listener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    ad->naviPageup();
  }

  void BackupDialog::pgdown_listener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    ad->naviPagedown();
  }

  void BackupDialog::return_listener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    ad->naviReturn();
  }

  static void BackupDialogKeyListener(void * src, SDL_Event * evt)
  {
    BackupDialog* ad = (BackupDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F1)
	  {
	    ad->backup_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F2)
	  {
	    ad->filter_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_UP)
	  {
	    ad->naviUp();
	  }
	else if(key->keysym.sym == SDLK_DOWN)
	  {
	    ad->naviDown();
	  }
	else if(key->keysym.sym == SDLK_RIGHT)
	  {
	    ad->naviRight();
	  }
	else if(key->keysym.sym == SDLK_LEFT)
	  {
	    ad->naviLeft();
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEDOWN || key->keysym.sym == SDLK_F6)
	  {
	    ad->pgdown_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_PAGEUP || key->keysym.sym == SDLK_F5)
	  {
	    ad->pgup_listener(src,evt);
	  }
      }
  }
  
  static void LabelMouseUpListener(void * src, SDL_Event * evt)
  {
    struct t_LoadDialogButtonEventContainer * lc;
    BackupDialog * ld;
    lc =(struct t_LoadDialogButtonEventContainer *)src;
    ld = (BackupDialog *)lc->theLoadDialog;
    
    if(lc->index==ld->getActiveRecipe())
      {
	ld->naviReturn();
	return;
      }
    
    if(lc->index < ld->getMaxRecipesToDisplay())
      {
	ld->setActiveRecipe(lc->index);
      }
  }

  static TextField * pTF_Filter4dirFilter=0;

  BackupDialog::BackupDialog(int sdlw,		\
			 int sdlh,		\
			 int camw,		\
			 int camh,		\
			 int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    unsigned short Rezepte_y;
    short Rezepte_w;
    short MLoadName_y;
    short x_space;

    this->Parent = parent;
    this->ActiveRecipe = 0;
    this->MaxRecipesToDisplay = 0;
    this->ActiveSavePage = 0;
    this->MaxSavePages = 0;
    this->tmpRezept = new Rezept();
    this->LoadMode = false;

    M_y = sdlh - yPos;
    if(M_y<=84)
      {
	MSpace_h = 2;
	MZeile_h = 18;
      }
    else
      {
	MSpace_h = 5;
	MZeile_h = 28;
      }

    x_space = 2;
    MLinks_x = sdlw/2 - 506;

    MLoadName_y  = yPos + 5*MSpace_h + 4*MZeile_h;
    Rezepte_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Rezepte_w = 125;

    theMenuBarSettings.Text[0]=(char *)"F1 backup";
    theMenuBarSettings.Text[1]=(char *)"F2 filter";
    theMenuBarSettings.Text[2]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.Text[3]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.Text[4]=(char *)"F5 prev";
    theMenuBarSettings.Text[5]=(char *)"F6 next";
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=backup_listener;
    theMenuBarSettings.evtFnks[1]=filter_listener;
    theMenuBarSettings.evtFnks[2]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.evtFnks[3]=0;//Nummer 2 und Nummer 3 sind in LoadMenueBar nicht vorhanden!
    theMenuBarSettings.evtFnks[4]=pgup_listener;
    theMenuBarSettings.evtFnks[5]=pgdown_listener;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;

    theMenu = new LoadMenuBar((int)MLinks_x,(int)MLoadName_y,(int)MZeile_h, \
			      (char*)"Load backup",				\
			      &this->theMenuBarSettings,Parent,\
			      BackupDialog::MaxRezeptFileLaenge);

    unsigned int ii = 0;
    //char tmpc[16] = { ' '};
    //tmpc[15]='\0';
    blankText = (char *)" ";
    Globals* global = Globals::getInstance();
    for(unsigned int i=0;i<BackupDialog::RezepteLen;i++)
      {
	if(ii>=BackupDialog::RezepteProZeile)
	  {
	    ii=0;
	    Rezepte_y += 1*MSpace_h + 1*MZeile_h;
	  }
	//	sprintf(DateiNamen[i],"%s",tmpc);                   //Text Buffer füllen
	pLabel_Rezepte[i] = new Label(blankText,			\
				      MLinks_x + ii*x_space + ii*Rezepte_w, \
				      Rezepte_y,			\
				      Rezepte_w,			\
				      MZeile_h,				\
				      Parent->WerteSet);
	pLabel_Rezepte[i]->setFont(global->getDefaultFont());
	LabelEvtContainers[i].theLoadDialog = (void *)this;
	LabelEvtContainers[i].index = i;
	pLabel_Rezepte[i]->setLMButtonUpEvtHandler(LabelMouseUpListener,(void*)&LabelEvtContainers[i]);
	ii++;
	this->addEvtTarget(pLabel_Rezepte[i]);
      }

    pTF_Filter4dirFilter = theMenu->TextField_Filter;

    this->pTSource = this;
    this->setKeyboardUpEvtHandler(BackupDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);
  }

  unsigned int BackupDialog::getMaxRecipesToDisplay()
  {
    return this->MaxRecipesToDisplay;
  }

  unsigned int BackupDialog::getActiveRecipe()
  {
    return this->ActiveRecipe;
  }

  /* \brief Makiert das aktive Label und demaskiert den Vorherigen.
   */ 
  void BackupDialog::setActiveRecipe(unsigned int nr)
  {
    pLabel_Rezepte[ActiveRecipe]->setBorder(false);
    pLabel_Rezepte[ActiveRecipe]->repaint();
    
    if(nr<0)                     //Begrenzungen
      nr=0;
    if(nr>=BackupDialog::RezepteLen)
      nr=BackupDialog::RezepteLen-1;
    if(nr>=this->MaxRecipesToDisplay)
      nr=this->MaxRecipesToDisplay-1;
    if(nr<0)
      {
	this->show(this->Parent->theGUI->getMainSurface());
	return;
      }

    ActiveRecipe = nr;
    pLabel_Rezepte[ActiveRecipe]->setBorder(true);
    pLabel_Rezepte[ActiveRecipe]->repaint();
    this->show(this->Parent->theGUI->getMainSurface());
  }

  void BackupDialog::addToActiveRecipe(int summand)
  {
    int nr = this->ActiveRecipe;
    nr += summand;
    this->setActiveRecipe(nr);
  }

  int dirBackupFilter(const struct dirent * dir)
  {
    int ret = 0;

    if(strlen(dir->d_name)>BackupDialog::MaxRezeptFileLaenge)
      {
	goto out_null;
      }
    if(strlen(dir->d_name)==1)
      {
	goto out_null;
      }
    if(strlen(dir->d_name)==2)
      {
	goto out_null;
      }
    if(strlen(pTF_Filter4dirFilter->getText())>0)
      {
	/*if(0==strncmp(dir->d_name,				\
		      pTF_Filter4dirFilter->getText(),		\
		      strlen(pTF_Filter4dirFilter->getText())))*/
	if(strstr(dir->d_name,pTF_Filter4dirFilter->getText()))
	  {
	    goto out_eins;
	  }
	else
	  {
	    goto out_null;
	  }
      }
    else
      {
	goto out_eins;
      }

  out_eins:
    ret = 1;
  out_null:

    return ret;
  }

  void BackupDialog::clearFilter(void)
  {
    pTF_Filter4dirFilter->setText((char*)"");
  }

  int BackupDialog::readSaveDirectory(char * dirName,unsigned int page)
  {
    struct dirent **namelist;
    int n;
    int fileToShow;

    //this->EvtTargets.Next = 0;     //alle FileLabels deaktivieren
    //this->Next = 0;                //alles nach dem Keylistener = 0
    //this->addEvtTarget(this);      //den Screen Key Listener bei sich selber anmelden!
    /*    this->Label_LadenName->Next = 0; //Laden Label anzeigen
    this->addEvtTarget(Label_LadenName);
    this->Label_MenuText->Next = 0; //Laden MenuText anzeigen
    this->addEvtTarget(Label_MenuText);
    */
    //theMenu->addToEvtTarget(this);

    this->ActiveSavePage = page;

    n = scandir(dirName, &namelist, dirBackupFilter, alphasort);

    if (n < 0)
      return -1;
    if (0==n)
      return 1;

    unsigned int ii = n;
    this->MaxSavePages = 0;
    while(ii>=BackupDialog::RezepteLen)
      {
	this->MaxSavePages++;
	ii-=BackupDialog::RezepteLen;
      }

    for(unsigned int i=0;i<BackupDialog::RezepteLen;i++)
      {
	pLabel_Rezepte[i]->setText(blankText);
      }

    fileToShow = page * BackupDialog::RezepteLen;
    if(fileToShow<=n)
      {
	//int tmp = 0;
	int i = 0;
	unsigned int i2 = 0;
	for(i=fileToShow,i2=0;i<n&&i2<BackupDialog::RezepteLen;i++,i2++)
	  {
	    strncpy(DateiNamen[i2],namelist[i]->d_name,BackupDialog::MaxRezeptFileLaenge);
	    pLabel_Rezepte[i2]->setText(DateiNamen[i2]); //neuen Text setzen
	    //pLabel_Rezepte[i2]->Next = 0;  // alles hinter diesem Label = 0
	    //this->addEvtTarget(pLabel_Rezepte[i2]);
	    MaxRecipesToDisplay = i2+1;
	  }
      }

    while(n--)
      {
	free(namelist[n]);
      }
    free(namelist);
    setActiveRecipe(0);
    return 0;
  }

  void BackupDialog::naviUp(){this->addToActiveRecipe(-BackupDialog::RezepteProZeile);}
  void BackupDialog::naviDown(){this->addToActiveRecipe(+BackupDialog::RezepteProZeile);} 
  void BackupDialog::naviLeft(){this->addToActiveRecipe(-1);}
  void BackupDialog::naviRight(){this->addToActiveRecipe(+1);}
  void BackupDialog::naviReturn()
  {
    if(this->LoadMode)
      {
	if(!tmpRezept->readFromFile(Parent->pcSaveFilePath,DateiNamen[ActiveRecipe]))
	  {
	    Rezept::copy(tmpRezept,Parent->theRezept);
	    //Rezept::copy();
	    Parent->showRezept(0);
	    Parent->showArbeitsDialog();
	    //printf("ArbeitsDialog Update Recipe Daten\n");
	  }
	else
	  {
	    this->Parent->showErrorDialog((char*)"Error Reading SaveFile");
	  }
      }
    else //delete Mode
      {
	snprintf(DeletePath,1024,"%s%s",\
		 Parent->pcSaveFilePath,\
		 DateiNamen[ActiveRecipe]);
	this->Parent->showFileDeleteAbfrageDialog(DeletePath,DateiNamen[ActiveRecipe]);
      }
  }

  void BackupDialog::naviPageup()
  {
    if(this->ActiveSavePage>=1)
      this->Parent->showBackupDialog();
  }

  void BackupDialog::naviPagedown()
  {
    if(this->ActiveSavePage<this->MaxSavePages)
      this->Parent->showBackupDialog();
  }

  void BackupMenuDialog::backupMenu_escape_listener(void * src, SDL_Event * evt)
  {
    BackupMenuDialog* ad = (BackupMenuDialog*)src;//KeyListener
    ad->Parent->showLoadDialog(0,true);//page 0; loadMode=true
  }

  void BackupMenuDialog::backupMenu_create_listener(void * src, SDL_Event * evt)
  {
    BackupMenuDialog* ad = (BackupMenuDialog*)src;//KeyListener
    ad->Parent->showCreateBackupDialog();
    if(ad->checkForUSBStick1()||ad->checkForUSBStick2())
      ad->Parent->showCreateBackupDialog();
    else
      ad->Parent->showFlexibleErrorDialog((char*)"can`t find USB mass storage device",\
					  ArbeitsDialog::BackupMenuDialogIsActive);
  }

  void BackupMenuDialog::backupMenu_load_listener(void * src, SDL_Event * evt)
  {
    BackupMenuDialog* ad = (BackupMenuDialog*)src;//KeyListener
    //ad->Parent->showBackupDialog();
    ad->Parent->showErrorDialog((char*)"backupMenu load failed");
  }
  
  void BackupMenuDialog::BackupMenuKeyListener(void * src, SDL_Event * evt)
  {
    BackupMenuDialog* ad = (BackupMenuDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    ////SDLMod mod = key->keysym.mod;
    //char zeichen = 0;

    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->backupMenu_escape_listener(src,evt);
	  }
	/*else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	    }*/
	else if(key->keysym.sym == SDLK_F5)
	  {
	    ad->backupMenu_create_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_F6)
	  {
	    ad->backupMenu_load_listener(src,evt);
	  }
      }
  }

  BackupMenuDialog::~BackupMenuDialog(){};
  BackupMenuDialog::BackupMenuDialog(int sdlw,				\
				   int sdlh,				\
				   int camw,				\
				   int camh,				\
				   int yPos,ArbeitsDialog * parent):Screen()
  {
    short M_y;
    short MLinks_x;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y,x1,x2,x3,x_space,button_breit,button_schmal;

    this->Parent = parent;
    
    M_y = sdlh - yPos;
    if(M_y<=84)
      {
	MSpace_h = 2;
	MZeile_h = 18;
      }
    else
      {
	MSpace_h = 5;
	MZeile_h = 28;
      }

    MLinks_x = sdlw/2 - 506;

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    button_breit = 400;
    button_schmal = 200;
    x_space = 6;
    x1 = MLinks_x;
    x2 = x1 + button_breit + x_space;
    x3 = x2 + button_schmal + x_space;

    this->STICKPATH1 = (char *)"/media/SERVICE_USB_STICK/";
    this->STICKPATH2 = (char *)"/media/KINGSTON/";

    this->BackupFilePrefix = (char *)"ExaktBackup_";

    snprintf(this->StepText,256,\
	     "create or load recipes backup:");
    Label_Step = new Label(this->StepText,				\
			   MLinks_x,Zeile1_y,506*2,MZeile_h,Parent->MenuSet);
    
  
    theMenuBarSettings.Text[0]=0;
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=(char *)"F5 create";
    theMenuBarSettings.Text[5]=(char *)"F6 load";
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=0;

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=backupMenu_create_listener;
    theMenuBarSettings.evtFnks[5]=backupMenu_load_listener;
    theMenuBarSettings.evtFnks[6]=backupMenu_escape_listener;
    theMenuBarSettings.evtFnks[7]=0;//return_listener;

    theMenu = new MenuBar((int)MLinks_x,(int)Zeile5_y,(int)MZeile_h,(char*)"Backup", \
			  &this->theMenuBarSettings,Parent);

    this->pTSource = this;//EvtTarget Quelle setzen, damit der EvtListener die Quelle mitteilen kann
    this->EvtTargetID=(char*)"BackupMenuDialog";
    this->setKeyboardUpEvtHandler(BackupMenuKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!

    addEvtTarget(Label_Step);
    theMenu->addToEvtTarget(this);
  }

  bool BackupMenuDialog::checkForUSBStick1(void)
  {
    struct stat st;

    if(stat(BackupMenuDialog::STICKPATH1,&st) == 0){
	if((st.st_mode & S_IFDIR) != 0){
	  //printf(" %s is present\n",BackupMenuDialog::STICKPATH1);
	  return true;
	}
    }
    return false;
  }

  bool BackupMenuDialog::checkForUSBStick2(void)
  {
    struct stat st;

    if(stat(BackupMenuDialog::STICKPATH2,&st) == 0){
      if((st.st_mode & S_IFDIR) != 0){
	//printf(" %s is present\n",BackupMenuDialog::STICKPATH2);
	return true;
      }
    }
    return false;
  }

  bool BackupMenuDialog::backupDirAlreadyExists(char * dirSuffix)
  {
    struct stat st;
    char tmp[256];
    bool ret = false;

    snprintf(tmp,256,"%s%s",BackupMenuDialog::STICKPATH1,getCompleteBackupName(dirSuffix));
    if(stat(tmp,&st) == 0){
      if((st.st_mode & S_IFDIR) != 0){
	  //printf(" Dir %s already exists\n",tmp);
	ret = true;
      }
    }

    snprintf(tmp,256,"%s%s",BackupMenuDialog::STICKPATH2,getCompleteBackupName(dirSuffix));
    if(stat(tmp,&st) == 0){
      if((st.st_mode & S_IFDIR) != 0){
	  //printf(" Dir %s already exists\n",tmp);
	ret = true;
      }
    }

    return ret;
  }

  char* BackupMenuDialog::getCompleteBackupName(char * dirSuffix)
  {
    snprintf(this->BackupDirName,64,"%s%s",this->BackupFilePrefix,dirSuffix);
    return this->BackupDirName;
  }

  char* BackupMenuDialog::getCompleteBackupPath(char * dirSuffix)
  {
    struct stat st;
    
    for(int i=0;i<126;i++)
      this->BackupPathName[0] = '\0'; 

    //StickPath1 ist aktiv
    if(stat(BackupMenuDialog::STICKPATH1,&st) == 0)
      {
	snprintf(this->BackupPathName,256,"%s%s%s",	\
		 BackupMenuDialog::STICKPATH1,		\
		 BackupMenuDialog::BackupFilePrefix,	\
		 dirSuffix);
      }
    else
      { //StickPath2 ist aktiv
	snprintf(this->BackupPathName,256,"%s%s%s",	\
		 BackupMenuDialog::STICKPATH2,		\
		 BackupMenuDialog::BackupFilePrefix,	\
		 dirSuffix);	
      }

    return this->BackupPathName;
  }

  int BackupMenuDialog::getAmountOfFilesInDir(char * dir)
  {
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(dir);
    if(dirp==NULL)
      return -1;
    while ((entry = readdir(dirp)) != NULL) {
      if (entry->d_type == DT_REG) { /* If the entry is a regular file */
	file_count++;
      }
    }
    closedir(dirp);
    return file_count;
  }

  void CreateBackupDialog::escape_listener(void * src, SDL_Event * evt)
  {
    CreateBackupDialog* ad = (CreateBackupDialog*)src;
    ad->Parent->showBackupMenuDialog();
  }

  void CreateBackupDialog::return_listener(void * src, SDL_Event * evt)
  {
    char * BackupDirPath;
    char cpBefehl[512];
    int filecount = 0;
    char tmp[512];

    CreateBackupDialog* ad = (CreateBackupDialog*)src;
    BackupMenuDialog* bm = ad->Parent->getBackupMenuDialog();


    //kein Name angegeben?
    if(strlen(ad->TextField_Name->getText())<=0){
      ad->Parent->showFlexibleErrorDialog((char*)"please enter backup dir name", \
					  ArbeitsDialog::CreateBackupDialogIsActive);
      return;
    }

    //Directory existiert bereits?
    if(bm->backupDirAlreadyExists(ad->TextField_Name->getText())){
      ad->TextField_Name->setText((char*)"");
      ad->Parent->showFlexibleErrorDialog((char*)"backup dir name already exists", \
					  ArbeitsDialog::CreateBackupDialogIsActive);
      return;
    }

    BackupDirPath = bm->getCompleteBackupPath(ad->TextField_Name->getText());
    //Directory erzeugen
    if(mkdir(BackupDirPath,0700)<0){
      ad->Parent->showFlexibleErrorDialog((char*)"cannot create backup dir", \
					  ArbeitsDialog::ArbeitsDialogIsActive);
      return;
    }

    filecount = bm->getAmountOfFilesInDir(ad->Parent->pcSaveFilePath);

    //Kopierbefehl
    snprintf(cpBefehl,512,"cp %s* %s",ad->Parent->pcSaveFilePath,BackupDirPath);

    //printf("directory created: %s\n",BackupDirPath);
    //printf("copy Befehl: %s copy %i files\n",cpBefehl,filecount);
    //printf("make sure there is enough space for %i recipes\n",filecount);

    //Erfolgsmeldung vorbereiten
    snprintf(tmp,512,"Created backup for %i recipes in \"%s\" .",	\
	     filecount,							\
	     bm->getCompleteBackupName(ad->TextField_Name->getText()));

    if(system(cpBefehl)<0)
      {
	ad->Parent->showFlexibleErrorDialog((char*)"copy recipes to backup dir failed", \
					  ArbeitsDialog::ArbeitsDialogIsActive);
	return;
      }
    else
      {
	ad->Parent->showFlexibleInfoDialog(tmp, \
					    ArbeitsDialog::BackupMenuDialogIsActive);
	return;
      }
  }

  void CreateBackupDialog::CreateBackupDialogKeyListener(void * src, SDL_Event * evt)
  {
    CreateBackupDialog* ad = (CreateBackupDialog*)src;//KeyListener
    SDL_KeyboardEvent * key = (SDL_KeyboardEvent *)&evt->key;
    if( key->type == SDL_KEYUP )
      {
	if(key->keysym.sym == SDLK_ESCAPE)
	  {
	    ad->escape_listener(src,evt);
	  }
	else if(key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER)
	  {
	    ad->return_listener(src,evt);
	  }
      }
  }

  CreateBackupDialog::CreateBackupDialog(int sdlw,					\
					 int sdlh,			\
					 int camw,			\
					 int camh,			\
					 int yPos,			\
					 ArbeitsDialog * parent):Screen()
  {
    short M_y;
    unsigned short MSpace_h;
    unsigned short MZeile_h;
    //unsigned short Rezepte_y;
    //short Rezepte_w;
    short Zeile1_y,Zeile2_y,Zeile3_y,Zeile4_y,Zeile5_y;
    short Spalte1_x, Spalte2_x, Spalte3_x;

    short Button_w = 332;
    short x_space = 8;

    this->Parent = parent;

    M_y = sdlh - yPos;
    if(M_y<=84)
      {
	MSpace_h = 2;
	MZeile_h = 18;
      }
    else
      {
	MSpace_h = 5;
	MZeile_h = 28;
      }

    Zeile1_y = yPos + 1*MSpace_h + 0*MZeile_h;
    Zeile2_y = yPos + 2*MSpace_h + 1*MZeile_h;
    Zeile3_y = yPos + 3*MSpace_h + 2*MZeile_h;
    Zeile4_y = yPos + 4*MSpace_h + 3*MZeile_h;
    Zeile5_y = yPos + 5*MSpace_h + 4*MZeile_h;
    //Rezepte_w = 108;

    Spalte1_x = sdlw/2 - 506;
    Spalte2_x = Spalte1_x + 1*Button_w+1*x_space;
    Spalte3_x = Spalte1_x + 2*Button_w+2*x_space;

    Label_Name = new Label("Enter backup name",\
			   Spalte1_x,Zeile1_y,Button_w,MZeile_h,Parent->MenuSet);

    Label_Info = new Label("Backup Name",Spalte3_x,Zeile1_y,Button_w,MZeile_h,Parent->DialogSet);

    TextField_Name = new TextField(0,CreateBackupDialog::BackupNameMaxLen, \
				   Spalte2_x,				\
				   Zeile1_y,Button_w,			\
				   MZeile_h,				\
				   Parent->WerteSet);
    TextField_Name->setFont(Globals::getFontButtonBig());
    TextField_Name->setActive(true);
    

    theMenuBarSettings.Text[0]=0;//(char *)"F1 huhuhu";
    theMenuBarSettings.Text[1]=0;
    theMenuBarSettings.Text[2]=0;
    theMenuBarSettings.Text[3]=0;
    theMenuBarSettings.Text[4]=0;
    theMenuBarSettings.Text[5]=0;
    theMenuBarSettings.Text[6]=(char *)"ESC";
    theMenuBarSettings.Text[7]=(char *)"ENTER";

    theMenuBarSettings.evtSource = (void*)this;

    theMenuBarSettings.evtFnks[0]=0;
    theMenuBarSettings.evtFnks[1]=0;
    theMenuBarSettings.evtFnks[2]=0;
    theMenuBarSettings.evtFnks[3]=0;
    theMenuBarSettings.evtFnks[4]=0;
    theMenuBarSettings.evtFnks[5]=0;
    theMenuBarSettings.evtFnks[6]=escape_listener;
    theMenuBarSettings.evtFnks[7]=return_listener;


    theMenu = new MenuBar((int)Spalte1_x,(int)Zeile5_y,(int)MZeile_h,(char*)"create backup", \
			  &this->theMenuBarSettings,Parent);

    this->pTSource = this;//EvtTarget Quelle setzen
    this->EvtTargetID=(char*)"CreateBackupDialog";
    this->setKeyboardUpEvtHandler(CreateBackupDialogKeyListener);
    this->addEvtTarget(this);//den Screen Key Listener bei sich selber anmelden!
    theMenu->addToEvtTarget(this);
    this->addEvtTarget(Label_Info);
    this->addEvtTarget(TextField_Name);
    this->addEvtTarget(Label_Name);
  }

}
