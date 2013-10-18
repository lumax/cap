/*
Bastian Ruppert
*/

namespace EuMax01
{

#ifndef __BACKUPDIALOG_H__
#define __BACKUPDIALOG_H__

  class ArbeitsDialog;

  /*  struct t_LoadDialogButtonEventContainer
  {
    void * theLoadDialog;
    unsigned int index;
    };*/

  class OkCancelDialog;

  class BackupDialog : public Screen
  {
  public:
    BackupDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
	       ArbeitsDialog * parent);

    ArbeitsDialog * Parent;
    static const unsigned int RezepteLen = 32;
    static const unsigned int RezepteProZeile = 8;
    static const unsigned int MaxRezeptFileLaenge = 15;
    void setActiveRecipe(unsigned int nr);
    void naviUp();
    void naviDown();
    void naviLeft();
    void naviRight();
    //void naviReturn();
    //void naviPageup();
    //void naviPagedown();
    int overwriteBackup();
    int skipBackup();

    OkCancelDialog * getOkCancelDialog();

    int readUSBStickDirectory(int page);

    Rezept * tmpRezept;

    static void escape_listener(void * src, SDL_Event * evt);
    //static void backup_listener(void * src, SDL_Event * evt);
    //static void filter_listener(void * src, SDL_Event * evt);
    //static void pgup_listener(void * src, SDL_Event * evt);
    //static void pgdown_listener(void * src, SDL_Event * evt);
    static void overwrite_listener(void * src,SDL_Event * evt);
    static void skip_listener(void * src,SDL_Event * evt);

    static int dirBackupFilter(const struct dirent * dir);    

    static void OkCancel_return_listener(void * src,SDL_Event * evt);
    static void OkCancel_escape_listener(void * src,SDL_Event * evt);

    static const int OverwriteIsActive = 1;
    static const int SkipIsActive = 2;
    static const int CleanupBeforeBackupIsActive = 3;

    void setDialogID(int id);
    int getDialogID();

    void prepareOkCancelDialog(int DialogID);

    unsigned int getMaxRecipesToDisplay();
    unsigned int getActiveRecipe();

    //void clearFilter(void);

    char DeletePath[1024];

  private:
    int DialogID;
    bool LoadMode;
    unsigned int ActiveRecipe;
    unsigned int MaxRecipesToDisplay;
    int ActiveSavePage;
    int MaxSavePages;
    void addToActiveRecipe(int summand);
    Label * pLabel_Rezepte[BackupDialog::RezepteLen];
    struct t_LoadDialogButtonEventContainer LabelEvtContainers[BackupDialog::RezepteLen];
    char DateiNamen[BackupDialog::RezepteLen][32];
    char * blankText;
    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
    OkCancelDialog * theOkCancelDialog;
  };

  class BackupMenuDialog : public Screen
  {
  public:
    BackupMenuDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		      ArbeitsDialog * parent);
    ~BackupMenuDialog();
    ArbeitsDialog * Parent;

    static void BackupMenuKeyListener(void * src, SDL_Event * evt);
    //void crosshairsKeyListener(SDL_KeyboardEvent * key);

    static void backupMenu_create_listener(void * src, SDL_Event * evt);
    static void backupMenu_load_listener(void * src, SDL_Event * evt);
    static void backupMenu_escape_listener(void * src, SDL_Event * evt);

    bool checkForUSBStick1(void);
    bool checkForUSBStick2(void);
    bool backupDirAlreadyExists(char * dirSuffix);
    char* getCompleteBackupName(char * dirSuffix);
    char* getCompleteBackupPath(char * dirSuffix);
    int getAmountOfFilesInDir(char * dir);

    const char * STICKPATH1;// = "/media/SERVICE_USB_STICK";
    const char * STICKPATH2;// = "/media/KINGSTON";
    const char * BackupFilePrefix;

  private:
    Label * Label_Step;
    char StepText[256];
    char BackupDirName[64];//"ExaktBackup_" + maximal 15 Zeichen (BackupNameMaxLen)
    char BackupPathName[256];//DirPath + BackupDirName

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };

  class CreateBackupDialog : public Screen
  {
  public:
    CreateBackupDialog(int sdlw,int sdlh, int camw,int camh,int yPos,\
		       ArbeitsDialog * parent);
    ArbeitsDialog * Parent;

    static void CreateBackupDialogKeyListener(void * src, SDL_Event * evt);

    static void escape_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);

    TextField * TextField_Name;
    MenuBar * theMenu;

  private:
    Label * Label_Name;

    Label * Label_Info;

    struct t_MenuBarSettings theMenuBarSettings;
    static const int BackupNameMaxLen = 15;
  };
  
#endif /* __BACKUPDIALOG_H__*/
}//end namespace
