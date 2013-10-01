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
    void naviReturn();
    void naviPageup();
    void naviPagedown();
    int readSaveDirectory(char * dirName,unsigned int page);

    Rezept * tmpRezept;

    static void escape_listener(void * src, SDL_Event * evt);
    static void backup_listener(void * src, SDL_Event * evt);
    static void filter_listener(void * src, SDL_Event * evt);
    static void pgup_listener(void * src, SDL_Event * evt);
    static void pgdown_listener(void * src, SDL_Event * evt);
    static void return_listener(void * src, SDL_Event * evt);
    
    unsigned int getMaxRecipesToDisplay();
    unsigned int getActiveRecipe();

    void clearFilter(void);

    char DeletePath[1024];

  private:
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
    LoadMenuBar * theMenu;
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

  private:
    Label * Label_Step;
    char StepText[256];

    struct t_MenuBarSettings theMenuBarSettings;
    MenuBar * theMenu;
  };

  
#endif /* __BACKUPDIALOG_H__*/
}//end namespace
