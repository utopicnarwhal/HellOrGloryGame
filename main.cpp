#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <math.h>

#define quasicos(x) (1-(((x)+1)&2))
#define quasitg(x) ((((x)&1)<<1)-1)
#define M1_POW(x) (1-(((x)&1)<<1))
#define naib(x,y) ((x)>(y)?(x):(y))

#define MAPMAXSIZEX 100
#define MAPMAXSIZEY 100

using namespace std;

enum KEYS{UP,DOWN,LEFT,RIGHT,SPACE};
enum STATE{MAINMENU,GAME};
enum BTTMS{BAG,SPELLS,INFO,QUESTBOOK,BACKTOGAME,SAVE,OPTIONS,QUITTOMAINMENU,ENLARGEDISPLAY,REDUSEDISPLAY,FULLSCREEN,CLOSEOPTIONS,VOLUMEUP,VOLUMEDOWN,NEWGAME,LOAD,MAINOPTIONS,EXIT,LINESKILLRIGHT,LINESKILLLEFT};
enum PROF{WARRIOR,ASSASIN,WIZARD};
enum DEAL{WALKUP,WALKDOWN,WALKLEFT,WALKRIGHT,NOTHING,ATTACK,DEATH};
enum PLAYERIMAGE{BODY,LEGS,TRUNK,HEAD,ARM,WEAPONIMAGE};
enum SIDE{HELL,GLORY};
enum EQUIPTYPE{HELM,ARMOR,GLOVES,LEGGINGS,WEAPON,RING,AMULET,ARTIFACT};
enum WEAPONTYPE{SWORD,BOW,STAFF};
enum SELECTTYPE{ENEMY,NPC};
enum ATTRIBUTES{STRENGTH,AGILITY,INTELLIGENCE,LUCKY};

inline int specfunc(int x){return ((((x>>1)^x)&1)-1)^x;}
#define translate(cd,d) (specfunc((cd)+specfunc(d))&3)

#define minicos(x) (1-((x)&(-(x))))
#define minisin(x) ((((x)+1)&(3-(x)))-1)

#include "header.h"

mapp loadmap(block blocks[4],object objects[4],item items[4],monster monsters[4],npc npcs[4]);
int walkatob(int landmind[MAPMAXSIZEX][MAPMAXSIZEY],int xa,int ya,int xb,int yb);

int main(int argc, char **argv)
{
    //VAR
    bool done = false,redraw = false,sttngsopen = false,menuopen = false;
    bool open[4]={false,false,false,false};
    float FPS = 60;
    int numbmap = 3;
    int width = 800,height = 600;
    int zoom = 1;
    int camdir = 0;
    int volume = 50;
    int gameFPS = 0,frames = 0;
    int selecter = 0;
    person *selectperson = NULL;
    short int focusx=0,focusy=0,selectx=0,selecty=0;
    int windx[4]={60,60,60,60};
    int windy[4]={150,150,150,150};
    int focuswind=-1;
    int selectlineskill=WARRIOR;
    int movemousex=0,movemousey=0;
    char sectorname[20];

    map0.numplayers=1;

    bool keys[5]={false,false,false,false,false};
    int state=MAINMENU;

    buttom bttms[20];

    //ALLEGRO VAR
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE  *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *calibri = NULL,*pixelcyr = NULL,*pixelcyrmenu = NULL,*pixelcyrmainmenu = NULL;
    ALLEGRO_CONFIG *cfg = NULL,*blockscfg = NULL,*objcfg = NULL,*monsterscfg = NULL,*itemscfg = NULL,*npccfg = NULL,*spellscfg = NULL;
    ALLEGRO_BITMAP *imagei = NULL;
    ALLEGRO_BITMAP *mainmenubackground = NULL;
    ALLEGRO_BITMAP *logo = NULL;
    ALLEGRO_BITMAP *mainmenubutton = NULL;
    ALLEGRO_BITMAP *sttngsimg = NULL;
    ALLEGRO_BITMAP *wndwframe = NULL;
    ALLEGRO_BITMAP *wndwbckgrnd = NULL;
    ALLEGRO_BITMAP *setter = NULL;
    ALLEGRO_BITMAP *checker = NULL;
    ALLEGRO_BITMAP *picker = NULL;
    ALLEGRO_BITMAP *selecterimg[2];
    ALLEGRO_BITMAP *areaselecter[2];
    ALLEGRO_BITMAP *arrowlineskill;
    ALLEGRO_BITMAP *minimap = NULL;
    ALLEGRO_BITMAP *statusbar = NULL;
    ALLEGRO_BITMAP *commbar = NULL;
    ALLEGRO_BITMAP *tab = NULL;
    ALLEGRO_BITMAP *bagimage = NULL,*spellsimage = NULL,*infoimage = NULL,*questbimage = NULL;
    ALLEGRO_BITMAP *pageimage = NULL;
    ALLEGRO_BITMAP *playerframe = NULL;
    ALLEGRO_BITMAP *coinimage = NULL;
    ALLEGRO_BITMAP *closeimage = NULL;
    ALLEGRO_BITMAP *attackindexsword = NULL,*armorindex = NULL;
    ALLEGRO_BITMAP *heart = NULL,*health = NULL,*bottle = NULL,*mana = NULL,*expball = NULL,*exper = NULL;
    ALLEGRO_BITMAP *healthbar = NULL,*barhealth = NULL;
    ALLEGRO_BITMAP *classicons = NULL;
    ALLEGRO_BITMAP *spllfrm = NULL;
    ALLEGRO_BITMAP *menuframe = NULL;
    ALLEGRO_BITMAP *menuback = NULL;
    ALLEGRO_BITMAP *menubutton = NULL;
    ALLEGRO_BITMAP *imgplus = NULL;
    ALLEGRO_BITMAP *gameoverimage = NULL;
    ALLEGRO_BITMAP *enteractwind = NULL;
    ALLEGRO_BITMAP *floorimage[4];
    ALLEGRO_BITMAP *glorycursor = NULL,*glorycursorarrows = NULL;
    ALLEGRO_BITMAP *hellcursor = NULL,*hellcursorarrows = NULL;
    ALLEGRO_SAMPLE *song = NULL;
    ALLEGRO_SAMPLE_INSTANCE *songInstance[10];
    ALLEGRO_MOUSE_STATE mousestate;

    if (!al_init()) //ALLEGRO INIT ERROR
    {
        al_show_native_message_box(NULL,NULL,NULL,"Failed to initialize allegro!",NULL,0);
        return -1;
    }
    al_init_font_addon();  //initialize addons and input
    al_init_ttf_addon();
    al_install_keyboard();
    al_init_image_addon();
    al_install_mouse();
    al_install_audio();
    al_init_acodec_addon();
    al_init_primitives_addon();
    srand(time(NULL));

    cfg = al_load_config_file("cfgs/config.cfg");
    if (cfg==NULL)                //CREATE CONFIG BY DEFAULT
    {
        cfg = al_create_config();
        al_add_config_comment(cfg,"","GAME SETTINGS");
        al_add_config_section(cfg,"display settings");
        al_set_config_value(cfg,"display settings","width","800");
        al_set_config_value(cfg,"display settings","height","600");
        al_set_config_value(cfg,"display settings","fullscreen","0");
        al_set_config_value(cfg,"volume settings","music","50");
    }

    width = atoi(al_get_config_value(cfg,"display settings","width"));  //LOAD FROM CONFIG
    height = atoi(al_get_config_value(cfg,"display settings","height"));
    volume = atoi(al_get_config_value(cfg,"volume settings","music"));
    if (atoi(al_get_config_value(cfg,"display settings","fullscreen"))==1)
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);

    display = al_create_display(width,height); //CREATE DISPLAY and WINDOW PARAMETERS
    al_set_display_icon(display,al_load_bitmap("img/icon.png"));
    al_set_window_title(display,"Hell or Glory");

    if (!display)  //CREATE DISPLAY ERROR
    {
        al_show_native_message_box(NULL,NULL,NULL,"Failed to initialize display!",NULL,0);
        return -1;
    }
    al_set_target_bitmap(al_get_backbuffer(display));

    {mainmenubackground = al_load_bitmap("img/menubackground.png");  // Loading's image
    al_draw_scaled_bitmap(mainmenubackground,0,0,800,600,0,0,width,height,0);
    logo = al_load_bitmap("img/logo.png");
    al_draw_scaled_bitmap(logo,0,0,al_get_bitmap_width(logo),al_get_bitmap_height(logo),0,0,width,width/4,0);
    al_flip_display();

    sttngsimg = al_load_bitmap("img/sttngs.png");
    setter = al_load_bitmap("img/setter.png");
    picker = al_load_bitmap("img/picker.png");
    selecterimg[ENEMY] = al_load_bitmap("img/enemyselecter.png");
    selecterimg[NPC] = al_load_bitmap("img/npcselecter.png");
    areaselecter[0]=al_load_bitmap("img/areaselecter.png");
    areaselecter[1]=al_load_bitmap("img/areaselecterno.png");
    checker = al_load_bitmap("img/checker.png");
    wndwframe = al_load_bitmap("img/wndwframe.png");
    wndwbckgrnd = al_load_bitmap("img/wndwbckgrnd.png");

    mainmenubutton = al_load_bitmap("img/mainmenubutton.png");
    minimap = al_load_bitmap("img/minimap.png");
    statusbar = al_load_bitmap("img/statusbar.png");
    classicons = al_load_bitmap("img/classicons.png");
    commbar = al_load_bitmap("img/commbar.png");
    tab = al_load_bitmap("img/tab.png");
    bagimage = al_load_bitmap("img/bag.png");
    spellsimage = al_load_bitmap("img/magicbook.png");
    infoimage = al_load_bitmap("img/i.png");
    questbimage = al_load_bitmap("img/questimg.png");
    pageimage = al_load_bitmap("img/page.png");
    coinimage = al_load_bitmap("img/coin.png");
    playerframe = al_load_bitmap("img/playerframe.png");
    closeimage = al_load_bitmap("img/close.png");
    attackindexsword = al_load_bitmap("img/attackindexsword.png");
    armorindex = al_load_bitmap("img/armorindex.png");
    heart = al_load_bitmap("img/heart.png");
    health = al_load_bitmap("img/hp.png");
    bottle = al_load_bitmap("img/bottle.png");
    mana = al_load_bitmap("img/mana.png");
    expball = al_load_bitmap("img/expball.png");
    exper = al_load_bitmap("img/exp.png");
    healthbar = al_load_bitmap("img/healthbar.png");
    barhealth = al_load_bitmap("img/barhealth.png");
    spllfrm = al_load_bitmap("img/spllfrm.png");
    arrowlineskill = al_load_bitmap("img/arrow.png");
    menuframe = al_load_bitmap("img/menuframe.png");
    menuback = al_load_bitmap("img/menuback.png");
    menubutton = al_load_bitmap("img/menubutton.png");
    imgplus = al_load_bitmap("img/plus.png");
    gameoverimage = al_load_bitmap("img/gameover.png");
    enteractwind = al_load_bitmap("img/enteractwind.png");

    glorycursor = al_load_bitmap("img/cursors/glorycursor.png");
    glorycursorarrows = al_load_bitmap("img/cursors/gloryarrows.png");
    hellcursor = al_load_bitmap("img/cursors/hellcursor.png");
    hellcursorarrows = al_load_bitmap("img/cursors/hellarrows.png");}

    {bttms[BAG].name = "bag"; // buttoms equally
    bttms[SPELLS].name = "spells";
    bttms[INFO].name = "info";
    bttms[QUESTBOOK].name = "questbook";
    bttms[BAG].image[0] = bagimage;
    bttms[SPELLS].image[0] = spellsimage;
    bttms[INFO].image[0] = infoimage;
    bttms[QUESTBOOK].image[0] = questbimage;
    bttms[BAG].image[1] = bttms[SPELLS].image[1] = bttms[INFO].image[1]= bttms[QUESTBOOK].image[1] = tab;
    bttms[BAG].visible = bttms[SPELLS].visible = bttms[INFO].visible = bttms[QUESTBOOK].visible = true;

    bttms[BAG].x = bttms[SPELLS].x = bttms[INFO].x= bttms[QUESTBOOK].x = -9;
    bttms[BAG].y = 225; bttms[SPELLS].y = 275; bttms[INFO].y = 325;
    bttms[BAG].width = bttms[SPELLS].width = bttms[INFO].width = bttms[QUESTBOOK].width = 50;
    bttms[BAG].height = bttms[SPELLS].height = bttms[INFO].height= bttms[QUESTBOOK].height = 50;

    bttms[BACKTOGAME].visible = bttms[SAVE].visible = bttms[OPTIONS].visible = bttms[QUITTOMAINMENU].visible = false;
    bttms[BACKTOGAME].x = bttms[SAVE].x =  bttms[OPTIONS].x = bttms[QUITTOMAINMENU].x = width/2-75;
    bttms[BACKTOGAME].y = height/2-132; bttms[SAVE].y = height/2-82;
    bttms[OPTIONS].y = height/2+50; bttms[QUITTOMAINMENU].y = height/2+100;
    bttms[BACKTOGAME].width = bttms[SAVE].width = bttms[OPTIONS].width = bttms[QUITTOMAINMENU].width = 150;
    bttms[BACKTOGAME].height = bttms[SAVE].height = bttms[OPTIONS].height = bttms[QUITTOMAINMENU].height = 32;
    bttms[BACKTOGAME].image[1] = bttms[SAVE].image[1] = bttms[OPTIONS].image[1] = bttms[QUITTOMAINMENU].image[1] = menubutton;
    bttms[BACKTOGAME].name = "Back to game";bttms[SAVE].name = "Save";
    bttms[OPTIONS].name = "Options";bttms[QUITTOMAINMENU].name = "Main menu";

    bttms[ENLARGEDISPLAY].visible=bttms[REDUSEDISPLAY].visible=bttms[FULLSCREEN].visible=bttms[CLOSEOPTIONS].visible=bttms[VOLUMEUP].visible=bttms[VOLUMEDOWN].visible=false;
    bttms[ENLARGEDISPLAY].x=width/2+96;bttms[REDUSEDISPLAY].x=width/2-128;bttms[FULLSCREEN].x=width/2-128;bttms[CLOSEOPTIONS].x=width/2+220;bttms[VOLUMEUP].x=width/2+96;bttms[VOLUMEDOWN].x=width/2-128;
    bttms[ENLARGEDISPLAY].y=height/2-132;bttms[REDUSEDISPLAY].y=height/2-132;bttms[FULLSCREEN].y=height/2-82;bttms[CLOSEOPTIONS].y=height/2-220;bttms[VOLUMEUP].y=bttms[VOLUMEDOWN].y=height/2+50;
    bttms[ENLARGEDISPLAY].width=32;bttms[REDUSEDISPLAY].width=32;bttms[VOLUMEUP].width=bttms[VOLUMEDOWN].width=32;
    bttms[FULLSCREEN].width=32;bttms[CLOSEOPTIONS].width=32;
    bttms[ENLARGEDISPLAY].height=32;bttms[REDUSEDISPLAY].height=32;bttms[VOLUMEUP].height=bttms[VOLUMEDOWN].height=32;
    bttms[FULLSCREEN].height=32;bttms[CLOSEOPTIONS].height=32;
    bttms[CLOSEOPTIONS].image[0]=closeimage;

    bttms[NEWGAME].visible=bttms[MAINOPTIONS].visible=bttms[LOAD].visible=bttms[EXIT].visible=true;
    bttms[NEWGAME].width=bttms[MAINOPTIONS].width=bttms[LOAD].width=bttms[EXIT].width=width/4;
    bttms[NEWGAME].height=bttms[MAINOPTIONS].height=bttms[LOAD].height=bttms[EXIT].height=height/12;
    bttms[NEWGAME].x=bttms[MAINOPTIONS].x=bttms[EXIT].x=bttms[LOAD].x=width/2-bttms[NEWGAME].width/2;

    bttms[LINESKILLLEFT].image[0]=bttms[LINESKILLRIGHT].image[0]=arrowlineskill;
    bttms[LINESKILLLEFT].width=bttms[LINESKILLRIGHT].width=bttms[LINESKILLLEFT].height=bttms[LINESKILLRIGHT].height=50;
    bttms[LINESKILLLEFT].x=bttms[LINESKILLRIGHT].x=bttms[LINESKILLLEFT].y=bttms[LINESKILLRIGHT].y=0;
    bttms[LINESKILLLEFT].visible=bttms[LINESKILLRIGHT].visible=true;
    bttms[LINESKILLLEFT].name="lineskillleft";bttms[LINESKILLRIGHT].name="lineskillright";

    for (int i=NEWGAME;i<=EXIT;i++)
    {
        bttms[i].y=width/4+25+(i-NEWGAME)*height/8;
    }
    bttms[NEWGAME].image[1]=bttms[LOAD].image[1]=bttms[MAINOPTIONS].image[1]=bttms[EXIT].image[1] = mainmenubutton;
    bttms[NEWGAME].name="New game";bttms[LOAD].name="Load game";bttms[MAINOPTIONS].name="Options";bttms[EXIT].name="Exit";}

    //   LOADING FROM CONFIGS
        // LOADING BLOCKS
        blockscfg = al_load_config_file("cfgs/blockscfg.cfg");
        int numblocks = atoi(al_get_config_value(blockscfg,"","numblocks"));
        block blocks[numblocks];
        ALLEGRO_BITMAP *blocksimage[numblocks];
        for (int i = 0; i<numblocks;i++)
        {
            itoa(i,sectorname,10);
            blocks[i].name = al_get_config_value(blockscfg,sectorname,"name");
            blocks[i].num = atoi(al_get_config_value(blockscfg,sectorname,"num"));
            blocks[i].firm = atoi(al_get_config_value(blockscfg,sectorname,"firm"));
            blocks[i].height = atoi(al_get_config_value(blockscfg,sectorname,"height"));
            blocks[i].width = atoi(al_get_config_value(blockscfg,sectorname,"width"));
            blocks[i].length = atoi(al_get_config_value(blockscfg,sectorname,"length"));
            blocks[i].floor = atoi(al_get_config_value(blockscfg,sectorname,"floor"));
            switch (atoi(al_get_config_value(blockscfg,sectorname,"enters")))
            {
            case 0:
                blocks[i].enters[0]=false;blocks[i].enters[1]=false;blocks[i].enters[2]=false;blocks[i].enters[3]=false;
                break;
            case 1:
                blocks[i].enters[0]=false;blocks[i].enters[1]=false;blocks[i].enters[2]=false;blocks[i].enters[3]=false;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter1"))-1]=true;
                break;
            case 2:
                blocks[i].enters[0]=false;blocks[i].enters[1]=false;blocks[i].enters[2]=false;blocks[i].enters[3]=false;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter1"))-1]=true;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter2"))-1]=true;
                break;
            case 3:
                blocks[i].enters[0]=false;blocks[i].enters[1]=false;blocks[i].enters[2]=false;blocks[i].enters[3]=false;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter1"))-1]=true;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter2"))-1]=true;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter3"))-1]=true;
                break;
            case 4:
                blocks[i].enters[0]=false;blocks[i].enters[1]=false;blocks[i].enters[2]=false;blocks[i].enters[3]=false;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter1"))-1]=true;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter2"))-1]=true;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter3"))-1]=true;
                blocks[i].enters[atoi(al_get_config_value(blockscfg,sectorname,"enter4"))-1]=true;
                break;
            case 5:
                blocks[i].enters[0]=true;blocks[i].enters[1]=true;blocks[i].enters[2]=true;blocks[i].enters[3]=true;
                break;
            }
            blocks[i].enterz = atoi(al_get_config_value(blockscfg,sectorname,"enterz"));
            blocks[i].stair = atoi(al_get_config_value(blockscfg,sectorname,"stair"));
            if (blocks[i].stair==1)
            {
                blocks[i].up=atoi(al_get_config_value(blockscfg,sectorname,"up"));
            }else blocks[i].up=0;
            blocksimage[i] = al_load_bitmap(al_get_config_value(blockscfg,sectorname,"image"));
            blocks[i].color = al_get_pixel(blocksimage[i],16,8);
        }
        // LOADING OBJECTS
        objcfg = al_load_config_file("cfgs/objectscfg.cfg");
        int numobj = atoi(al_get_config_value(objcfg,"","numobjects"));
        object objects[numobj];
        ALLEGRO_BITMAP *objimage[numobj];
        for (int i = 0;i<numobj;i++)
        {
            itoa(i,sectorname,10);
            objects[i].name = al_get_config_value(objcfg,sectorname,"name");
            objects[i].num = atoi(al_get_config_value(objcfg,sectorname,"num"));
            objects[i].firm = atoi(al_get_config_value(objcfg,sectorname,"firm"));
            objects[i].animation = atoi(al_get_config_value(objcfg,sectorname,"animation"));
            if (objects[i].animation>=1) objects[i].nu = atoi(al_get_config_value(objcfg,sectorname,"nu"));
            objects[i].height = atoi(al_get_config_value(objcfg,sectorname,"height"));
            objimage[i] = al_load_bitmap(al_get_config_value(objcfg,sectorname,"image"));
        }
        // LOADING MONSTERS
        monsterscfg = al_load_config_file("cfgs/monsterscfg.cfg");
        int nummonsters = atoi(al_get_config_value(monsterscfg,"","nummonsters"));
        monster monsters[nummonsters];
        ALLEGRO_BITMAP *monsterimage[nummonsters];
        for (int i = 0;i<nummonsters;i++)
        {
            itoa(i,sectorname,10);
            monsters[i].name = al_get_config_value(monsterscfg,sectorname,"name");
            monsters[i].num = atoi(al_get_config_value(monsterscfg,sectorname,"num"));
            monsters[i].height = atoi(al_get_config_value(monsterscfg,sectorname,"height"));
            monsters[i].armor = atoi(al_get_config_value(monsterscfg,sectorname,"armor"));
            monsters[i].magicresist = atoi(al_get_config_value(monsterscfg,sectorname,"magicresist"));
            monsters[i].maxhp = atoi(al_get_config_value(monsterscfg,sectorname,"maxhp"));
            monsters[i].sps = atoi(al_get_config_value(monsterscfg,sectorname,"sps"));
            monsters[i].damage = atoi(al_get_config_value(monsterscfg,sectorname,"damage"));
            monsters[i].aps = atoi(al_get_config_value(monsterscfg,sectorname,"aps"));
            monsters[i].agrrange = atoi(al_get_config_value(monsterscfg,sectorname,"agrrange"));
            monsterimage[i] = al_load_bitmap(al_get_config_value(monsterscfg,sectorname,"image"));
        }
        //LOADING ITEMS
        itemscfg = al_load_config_file("cfgs/itemscfg.cfg");
        int numitems = atoi(al_get_config_value(itemscfg,"","numitems"));
        item items[numitems];
        for (int i = 0;i<numitems;i++)
        {
            itoa(i,sectorname,10);
            items[i].name = al_get_config_value(itemscfg,sectorname,"name");
            items[i].num = atoi(al_get_config_value(itemscfg,sectorname,"num"));
            items[i].equiptype = atoi(al_get_config_value(itemscfg,sectorname,"equiptype"));
            items[i].armor = atoi(al_get_config_value(itemscfg,sectorname,"armor"));
            items[i].damage = atoi(al_get_config_value(itemscfg,sectorname,"damage"));
            items[i].cost = atoi(al_get_config_value(itemscfg,sectorname,"cost"));
            items[i].icon = al_load_bitmap(al_get_config_value(itemscfg,sectorname,"itemicon"));
            items[i].image = al_load_bitmap(al_get_config_value(itemscfg,sectorname,"itemimage"));
        }
        //LOADING NPC
        npccfg = al_load_config_file("cfgs/npccfg.cfg");
        int numnpc = atoi(al_get_config_value(npccfg,"","numnpc"));
        npc npcs[numnpc];
        for (int i = 0;i<numnpc;i++)
        {
            itoa(i,sectorname,10);
            npcs[i].name = al_get_config_value(npccfg,sectorname,"name");
            npcs[i].num = atoi(al_get_config_value(npccfg,sectorname,"num"));
            npcs[i].image = al_load_bitmap(al_get_config_value(npccfg,sectorname,"image"));
        }
        //LOADING SPELLS
        spellscfg = al_load_config_file("cfgs/spellscfg.cfg");
        int numspells = atoi(al_get_config_value(spellscfg,"","numspells"));
        spell spells[numspells];
        for (int i = 0;i<numspells;i++)
        {
            itoa(i,sectorname,10);
            spells[i].name = al_get_config_value(spellscfg,sectorname,"name");
            spells[i].num = atoi(al_get_config_value(spellscfg,sectorname,"num"));
            spells[i].icon = al_load_bitmap(al_get_config_value(spellscfg,sectorname,"icon"));
            spells[i].passive = atoi(al_get_config_value(spellscfg,sectorname,"passive"));
            if (spells[i].passive==0)
            {spells[i].manacost = atoi(al_get_config_value(spellscfg,sectorname,"manacost"));}
            spells[i].doingnum=atoi(al_get_config_value(spellscfg,sectorname,"doingnum"));
            spells[i].doing=al_get_config_value(spellscfg,sectorname,"doing");
            if (strcmp(al_get_config_value(spellscfg,sectorname,"lineskill"),"warrior")==0) spells[i].lineskill=WARRIOR;
            else if (strcmp(al_get_config_value(spellscfg,sectorname,"lineskill"),"assasin")==0) spells[i].lineskill=ASSASIN;
            else if (strcmp(al_get_config_value(spellscfg,sectorname,"lineskill"),"wizard")==0) spells[i].lineskill=WIZARD;
            else spells[i].lineskill=-1;
            spells[i].row = atoi(al_get_config_value(spellscfg,sectorname,"row"));
            spells[i].description = al_get_config_value(spellscfg,sectorname,"description");
        }


    hero* h=new hero(map0.pnewhero);

    h->equipment[HELM].itemm=items[0]; //player equally
    h->equipment[HELM].enabled=true;
    h->equipment[ARMOR].itemm=items[1];
    h->equipment[ARMOR].enabled=true;
    h->equipment[GLOVES].itemm=items[2];
    h->equipment[GLOVES].enabled=true;
    h->equipment[LEGGINGS].itemm=items[3];
    h->equipment[LEGGINGS].enabled=true;
    h->equipment[WEAPON].itemm=items[4];
    h->equipment[WEAPON].enabled=true;
    h->equipment[5].enabled=false;
    h->equipment[6].enabled=false;
    h->equipment[7].enabled=false;
    h->image[BODY] = al_load_bitmap("img/player/player.png");
    if (h->equipment[ARMOR].enabled==true) h->image[TRUNK] = h->equipment[ARMOR].itemm.image;
    if (h->equipment[LEGGINGS].enabled==true) h->image[LEGS] = h->equipment[LEGGINGS].itemm.image;
    if (h->equipment[HELM].enabled==true) h->image[HEAD] = h->equipment[HELM].itemm.image;
    if (h->equipment[GLOVES].enabled==true) h->image[ARM] = h->equipment[GLOVES].itemm.image;
    if (h->equipment[WEAPON].enabled==true) h->image[WEAPONIMAGE] = h->equipment[WEAPON].itemm.image;
    for (int m=0;m<4;m++)
        for (int n=0;n<3;n++)
            h->inventory[m][n].enabled=false;
    h->dir=DOWN;h->alive=true;
    h->name="player";
    h->x=10; h->y=12; h->z=0;
    h->exp = 0;h->needexp=100;
    h->attributes[0]=h->attributes[1]=h->attributes[2]=h->attributes[3]=10;
    h->hp=h->maxhp=h->attributes[STRENGTH]*11;h->hpregen=h->attributes[STRENGTH]*0.1;
    h->maxmp=h->attributes[INTELLIGENCE]*10;h->mp=40;h->mpregen=h->attributes[INTELLIGENCE]*0.1;
    h->busy=false;
    h->deal=NOTHING;
    h->choosingarea=0;
    h->damage = h->equipment[WEAPON].itemm.damage;
    h->stpcnt=0; h->sps=1.8;
    h->attckcnt=0;h->aps = 2;
    h->magicresist=25;
    h->getarmor();
    h->lvl=1;
    h->hpregencount=h->mpregencount=0;
    h->money = 0;
    h->spellpoints=5;
    h->attribpoints=10;
    for (int i=0;i<numspells;i++)
        h->spellslvl[i]=0;
    h->next = NULL;

    FILE *mapf=fopen("maps/map1.txt","rt");
    if (mapf==NULL) puts("Can't open file");

    fscanf(mapf,"%d*%d",&map0.sizex,&map0.sizey);


    //Loading map
    for (int j=0;j<map0.sizey;j++)
    {
        for (int i=0;i<map0.sizex;i++)
        {
            fscanf(mapf,"%d",&map0.coord[i][j].num);
            map0.coord[i][j]=blocks[map0.coord[i][j].num];
            fscanf(mapf,",%d",&map0.coord[i][j].dir);
        }
    }
    fscanf(mapf,"%dobjects",&map0.numobj); //loading obj
    for (int i = 0;i<map0.numobj;i++)
    {
        object* o=new object(map0.pnewobj);
        fscanf(mapf,"%d(%d;%d)",&o->num,&o->x,&o->y);
        o->name=objects[o->num].name;
        o->firm=objects[o->num].firm;
        o->height=objects[o->num].height;
        o->animation=objects[o->num].animation;
        if (o->animation>=1) o->nu=objects[o->num].nu;
        else o->nu=0;
        o->counter=0;
        o->z=0;
        o->next=NULL;
    }
    fscanf(mapf,"%dmonsters",&map0.nummonsters); //loading monsters
    for (int i = 0;i<map0.nummonsters;i++)
    {
        monster* m=new monster(map0.pnewmonster);
        fscanf(mapf,"%hu(%hu;%hu;%hu)",&m->num,&m->x,&m->y,&m->dir);
        m->name=monsters[m->num].name;
        m->height=monsters[m->num].height;
        m->maxhp=monsters[m->num].maxhp;
        m->armor=monsters[m->num].armor;
        m->magicresist=monsters[m->num].magicresist;
        m->sps=monsters[m->num].sps;
        m->aps=monsters[m->num].aps;
        m->agrrange=monsters[m->num].agrrange;
        m->damage=monsters[m->num].damage;
        m->stpcnt=0;m->attckcnt=0;m->dthcnt=0;
        m->busy=false;m->alive=true;m->deal=NOTHING;
        m->hp=m->maxhp;
        m->next=NULL;
    }
    fscanf(mapf,"%dnpc",&map0.numnpc); //loading NPC
    for (int i = 0 ;i<map0.numnpc;i++)
    {
        npc* n=new npc(map0.pnewnpc);
        fscanf(mapf,"%hd(%hd;%hd)",&n->num,&n->x,&n->y);
        n->name=npcs[n->num].name;
        n->image=npcs[n->num].image;
        n->alive=true;
        n->z=map0.coord[n->x][n->y].enterz;
        n->next=NULL;
    }
    fclose(mapf);
    //floor images
    for (int d=0;d<4;d++)
    {
        floorimage[d] = al_create_bitmap((map0.sizex+map0.sizey)*16,(map0.sizex+map0.sizey)*8);
        al_set_target_bitmap(floorimage[d]);

        for (int i=(d<2)?(map0.sizex-1):(0);(d<2)?(i>=0):(i<map0.sizex);(d<2)?(i--):(i++))
        {
            for (int j=(d%3==0)?(0):(map0.sizey-1);(d%3==0)?(j<map0.sizey):(j>=0);(d%3==0)?(j++):(j--))
            {
                map0.drawfloor(i,j,blocksimage,objimage,d);
            }
        }
    }
    al_set_target_bitmap(al_get_backbuffer(display));

    event_queue = al_create_event_queue();     //object's equally
    timer = al_create_timer(1.0 / FPS);
    calibri = al_load_font("font/calibri.ttf",18,0);
    pixelcyr = al_load_font("font/pixel cyr.ttf",12,0);
    pixelcyrmenu = al_load_font("font/pixel cyr.ttf",18,0);
    pixelcyrmainmenu = al_load_font("font/pixel cyr.ttf",height/33,0);
    al_set_mouse_cursor(display,al_create_mouse_cursor(glorycursor,1,1));

    al_reserve_samples(10);//song samples
    song = al_load_sample("audio/nodens-field-song.ogg");
    songInstance[0] = al_create_sample_instance(song);
    song = al_load_sample("audio/metal-falling-on-stone.ogg");
    songInstance[1] = al_create_sample_instance(song);
    al_set_sample_instance_playmode(songInstance[0],ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_playmode(songInstance[1],ALLEGRO_PLAYMODE_ONCE);

    al_attach_sample_instance_to_mixer(songInstance[0],al_get_default_mixer());
    al_attach_sample_instance_to_mixer(songInstance[1],al_get_default_mixer());

    al_set_sample_instance_gain(songInstance[0],float(volume)/10);
    al_set_sample_instance_gain(songInstance[1],float(volume)/10);
    al_play_sample_instance(songInstance[0]);

    al_register_event_source(event_queue, al_get_keyboard_event_source());//event queue and timer
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
    int gameTime = int(al_current_time());

    while (!done) //main game loop
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue,&ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)//key pressed
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_W:
                keys[translate(camdir,UP)]=true;
                break;
            case ALLEGRO_KEY_S:
                keys[translate(camdir,DOWN)]=true;
                break;
            case ALLEGRO_KEY_D:
                keys[translate(camdir,RIGHT)]=true;
                break;
            case ALLEGRO_KEY_A:
                keys[translate(camdir,LEFT)]=true;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE]=true;
                break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP)//key unpressed
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_W:
                keys[translate(camdir,UP)]=false;
                break;
            case ALLEGRO_KEY_S:
                keys[translate(camdir,DOWN)]=false;
                break;
            case ALLEGRO_KEY_D:
                keys[translate(camdir,RIGHT)]=false;
                break;
            case ALLEGRO_KEY_A:
                keys[translate(camdir,LEFT)]=false;
                break;
            case ALLEGRO_KEY_E:
                if (keys[UP]==false && keys[DOWN]==false && keys[LEFT]==false && keys[RIGHT]==false)
                    camdir=(camdir+1)%4;
                break;
            case ALLEGRO_KEY_Q:
                if (keys[UP]==false && keys[DOWN]==false && keys[LEFT]==false && keys[RIGHT]==false)
                    camdir=(camdir+3)%4;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE]=false;
                break;
            case ALLEGRO_KEY_ESCAPE:
                if (menuopen == false and sttngsopen == false)
                {
                    menuopen = true;
                    bttms[BACKTOGAME].visible = bttms[SAVE].visible = bttms[OPTIONS].visible = bttms[QUITTOMAINMENU].visible = true;
                    }
                else if (menuopen == true)
                {
                    menuopen = false;
                    bttms[BACKTOGAME].visible = bttms[SAVE].visible = bttms[OPTIONS].visible = bttms[QUITTOMAINMENU].visible = false;
                }
                else if (sttngsopen == true)
                {
                    sttngsopen=false;
                    bttms[ENLARGEDISPLAY].visible=bttms[REDUSEDISPLAY].visible=bttms[FULLSCREEN].visible=bttms[CLOSEOPTIONS].visible=false;
                }
                break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        { //mouse event
            al_get_mouse_state(&mousestate);

            if (mousestate.z>0 && state!=MAINMENU) // mouse wheel zoom
            {
                if (mousestate.z<5)
                zoom = mousestate.z;
                else al_set_mouse_z(4);
            }
                else al_set_mouse_z(1);

            //focusx and focusy
            if (state == GAME) map0.focusing(&focusx,&focusy,*map0.firsthero,width,height,zoom,mousestate,camdir);

            for (int i=BAG;i<=QUESTBOOK && state==GAME;i++)
            {
                //move tabs windows
                if (((mousestate.x>windx[i]+229 && mousestate.x<windx[i]+248 && mousestate.y>windy[i]+3 && mousestate.y<windy[i]+21 && focuswind==-1) or (movemousex!=0 && movemousey!=0 && focuswind==i)) && open[i]==true && mousestate.buttons & 1)
                {
                    if (movemousex==0 && movemousey==0)
                    {
                        focuswind=i;
                        movemousex=mousestate.x;movemousey=mousestate.y;al_set_mouse_cursor(display,al_create_mouse_cursor(glorycursorarrows,11,11));
                    }
                    windx[i]+=mousestate.x-movemousex;windy[i]+=mousestate.y-movemousey;
                    movemousex=mousestate.x;movemousey=mousestate.y;
                    break;
                }
                else if (movemousex!=0 && movemousey!=0 && open[i]==true && focuswind==i)
                {
                    focuswind=-1;
                    movemousex=0;movemousey=0;
                    al_set_mouse_cursor(display,al_create_mouse_cursor(glorycursor,1,1));
                    break;
                }
            }
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                if (state==GAME)
                {
                    if ((mousestate.x>bttms[REDUSEDISPLAY].x && mousestate.y>bttms[REDUSEDISPLAY].y && mousestate.x<bttms[REDUSEDISPLAY].x+bttms[REDUSEDISPLAY].width && mousestate.y<bttms[REDUSEDISPLAY].y+bttms[REDUSEDISPLAY].height && sttngsopen==true) && mousestate.buttons & 1)
                    { // reduce display
                        switch (atoi(al_get_config_value(cfg,"display settings","width")))
                        {
                        case 800:
                            al_set_config_value(cfg,"display settings","width","640");
                            break;
                        case 1024:
                            al_set_config_value(cfg,"display settings","width","800");
                            break;
                        case 1280:
                            al_set_config_value(cfg,"display settings","width","1024");
                            break;
                        }
                        switch (atoi(al_get_config_value(cfg,"display settings","height")))
                        {
                        case 600:
                            al_set_config_value(cfg,"display settings","height","480");
                            break;
                        case 768:
                            al_set_config_value(cfg,"display settings","height","600");
                            break;
                        case 720:
                            al_set_config_value(cfg,"display settings","height","768");
                            break;
                        }
                    }
                    else if ((mousestate.x>bttms[ENLARGEDISPLAY].x && mousestate.y>bttms[ENLARGEDISPLAY].y && mousestate.x<bttms[ENLARGEDISPLAY].x+bttms[ENLARGEDISPLAY].width && mousestate.y<bttms[ENLARGEDISPLAY].y+bttms[ENLARGEDISPLAY].height && sttngsopen==true) && mousestate.buttons & 1)
                    { // enlarge display
                        switch (atoi(al_get_config_value(cfg,"display settings","width")))
                        {
                        case 640:
                            al_set_config_value(cfg,"display settings","width","800");
                            break;
                        case 800:
                            al_set_config_value(cfg,"display settings","width","1024");
                            break;
                        case 1024:
                            al_set_config_value(cfg,"display settings","width","1280");
                            break;
                        }
                        switch (atoi(al_get_config_value(cfg,"display settings","height")))
                        {
                        case 480:
                            al_set_config_value(cfg,"display settings","height","600");
                            break;
                        case 600:
                            al_set_config_value(cfg,"display settings","height","768");
                            break;
                        case 768:
                            al_set_config_value(cfg,"display settings","height","720");
                            break;
                        }
                    }

                    else if ((mousestate.x>bttms[FULLSCREEN].x && mousestate.y>bttms[FULLSCREEN].y && mousestate.x<bttms[FULLSCREEN].x+bttms[FULLSCREEN].width && mousestate.y<bttms[FULLSCREEN].y+bttms[FULLSCREEN].height && sttngsopen==true) && mousestate.buttons & 1)
                    { // fullscreen
                        if (atoi(al_get_config_value(cfg,"display settings","fullscreen")))
                        al_set_config_value(cfg,"display settings","fullscreen","0");
                        else al_set_config_value(cfg,"display settings","fullscreen","1");
                    }
                    else if ((mousestate.x>bttms[VOLUMEUP].x && mousestate.y>bttms[VOLUMEUP].y && mousestate.x<bttms[VOLUMEUP].x+bttms[VOLUMEUP].width && mousestate.y<bttms[VOLUMEUP].y+bttms[VOLUMEUP].height && sttngsopen==true) && mousestate.buttons & 1)
                    { // volume up
                        if (volume<10) {volume+=1;al_set_sample_instance_gain(songInstance[0],float(volume)/10);}
                    }
                    else if ((mousestate.x>bttms[VOLUMEDOWN].x && mousestate.y>bttms[VOLUMEDOWN].y && mousestate.x<bttms[VOLUMEDOWN].x+bttms[VOLUMEDOWN].width && mousestate.y<bttms[VOLUMEDOWN].y+bttms[VOLUMEDOWN].height && sttngsopen==true) && mousestate.buttons & 1)
                    { //volume down
                        if (volume>0) {volume-=1;al_set_sample_instance_gain(songInstance[0],float(volume)/10);}
                    }
                    else if ((mousestate.x>bttms[CLOSEOPTIONS].x && mousestate.y>bttms[CLOSEOPTIONS].y && mousestate.x<bttms[CLOSEOPTIONS].x+bttms[CLOSEOPTIONS].width && mousestate.y<bttms[CLOSEOPTIONS].y+bttms[CLOSEOPTIONS].height && sttngsopen==true) && mousestate.buttons & 1)
                    { //close options
                        sttngsopen=false;
                        sttngsopen=false;
                        bttms[ENLARGEDISPLAY].visible=bttms[REDUSEDISPLAY].visible=bttms[FULLSCREEN].visible=bttms[CLOSEOPTIONS].visible=false;
                    }

                    if ((mousestate.x>bttms[OPTIONS].x && mousestate.x<bttms[OPTIONS].x+bttms[OPTIONS].width && mousestate.y>bttms[OPTIONS].y && mousestate.y<bttms[OPTIONS].y+bttms[OPTIONS].height && menuopen==true) && mousestate.buttons & 1)
                    { //sttngsopen
                        menuopen = false;
                        sttngsopen=true;
                        bttms[ENLARGEDISPLAY].visible=bttms[REDUSEDISPLAY].visible=bttms[FULLSCREEN].visible=bttms[CLOSEOPTIONS].visible=true;
                        bttms[BACKTOGAME].visible = bttms[SAVE].visible = bttms[OPTIONS].visible = bttms[QUITTOMAINMENU].visible = false;
                    }
                    else if ((mousestate.x>bttms[BACKTOGAME].x && mousestate.x<bttms[BACKTOGAME].x+bttms[BACKTOGAME].width && mousestate.y>bttms[BACKTOGAME].y && mousestate.y<bttms[BACKTOGAME].y+bttms[BACKTOGAME].height && menuopen==true) && mousestate.buttons & 1)
                    { //back to game
                        menuopen = false;
                        bttms[BACKTOGAME].visible = bttms[SAVE].visible = bttms[OPTIONS].visible = bttms[QUITTOMAINMENU].visible = false;
                    }
                    else if ((mousestate.x>bttms[SAVE].x && mousestate.x<bttms[SAVE].x+bttms[SAVE].width && mousestate.y>bttms[SAVE].y && mousestate.y<bttms[SAVE].y+bttms[SAVE].height && menuopen==true) && mousestate.buttons & 1)
                    { //save

                    }
                    else if ((mousestate.x>bttms[QUITTOMAINMENU].x && mousestate.x<bttms[QUITTOMAINMENU].x+bttms[QUITTOMAINMENU].width && mousestate.y>bttms[QUITTOMAINMENU].y && mousestate.y<bttms[QUITTOMAINMENU].y+bttms[QUITTOMAINMENU].height && menuopen==true) && mousestate.buttons & 1)
                    { //quit to main menu
                        menuopen=false;
                        for (int i=NEWGAME;i<=EXIT;i++)
                            bttms[i].visible=true;
                        state=MAINMENU;
                    }

                    if ((mousestate.x>bttms[BAG].x && mousestate.x<bttms[BAG].x+bttms[BAG].width && mousestate.y>bttms[BAG].y && mousestate.y<bttms[BAG].y+bttms[BAG].height) && mousestate.buttons & 1)
                    { //bag tab
                        if (open[BAG]==false)
                        {
                            open[BAG]=true;bttms[BAG].image[0] = bttms[BAG].image[1];
                        }
                        else
                        {
                            open[BAG]=false;bttms[BAG].image[0] = bagimage;
                        }
                    }
                    else if ((mousestate.x>bttms[SPELLS].x && mousestate.x<bttms[SPELLS].x+bttms[SPELLS].width && mousestate.y>bttms[SPELLS].y && mousestate.y<bttms[SPELLS].y+bttms[SPELLS].height) && mousestate.buttons & 1)
                    { //spells tab
                        if (open[SPELLS]==false)
                        {
                            open[SPELLS]=true;bttms[SPELLS].image[0] = bttms[SPELLS].image[1];
                        }
                        else
                        {
                            open[SPELLS]=false;bttms[SPELLS].image[0] = spellsimage;
                        }
                    }
                    else if ((mousestate.x>bttms[INFO].x && mousestate.x<bttms[INFO].x+bttms[INFO].width && mousestate.y>bttms[INFO].y && mousestate.y<bttms[INFO].y+bttms[INFO].height) && mousestate.buttons & 1)
                    { //info tab
                        if (open[INFO]==false)
                        {
                            open[INFO]=true;bttms[INFO].image[0] = bttms[INFO].image[1];
                        }
                        else
                        {
                            open[INFO]=false;bttms[INFO].image[0] = infoimage;
                        }
                    }
                    else if ((mousestate.x>bttms[QUESTBOOK].x && mousestate.x<bttms[QUESTBOOK].x+bttms[QUESTBOOK].width && mousestate.y>bttms[QUESTBOOK].y && mousestate.y<bttms[QUESTBOOK].y+bttms[QUESTBOOK].height) && mousestate.buttons & 1)
                    { //questbook tab
                        if (open[QUESTBOOK]==false)
                        {
                            open[QUESTBOOK]=true;bttms[QUESTBOOK].image[0] = bttms[QUESTBOOK].image[1];
                        }
                        else
                        {
                            open[QUESTBOOK]=false;bttms[QUESTBOOK].image[0] = questbimage;
                        }
                    }

                    if (open[BAG]==true)
                    {
                        for (int m=0;m<2;m++) //slots for equiping
                        {
                            for(int n=0;n<4;n++)
                            {
                                if ((mousestate.x>windx[BAG]+10+m*170 && mousestate.x<windx[BAG]+60+m*170 && mousestate.y>windy[BAG]+6+n*55 && mousestate.y<windy[BAG]+56+n*55 && map0.firsthero->equipment[m*4+n].enabled==true) && mousestate.buttons & 2)
                                {
                                    for (int i=0;i<12;i++)
                                    {
                                        if (map0.firsthero->inventory[i/4][i%4].enabled==false)
                                        {
                                            map0.firsthero->inventory[i/4][i%4].itemm=map0.firsthero->equipment[m*4+n].itemm;
                                            map0.firsthero->inventory[i/4][i%4].enabled=true;
                                            map0.firsthero->equipment[m*4+n].enabled=false;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        for (int m=0;m<3;m++) //slots
                        {
                            for (int n=0;n<4;n++)
                            {
                                if ((mousestate.x>windx[BAG]+4+n*50 && mousestate.x<windx[BAG]+54+n*50 && mousestate.y>windy[BAG]+246+m*50 && mousestate.y<windy[BAG]+296+m*50 && map0.firsthero->inventory[n][m].enabled==true) && mousestate.buttons & 2)
                                {
                                    if (map0.firsthero->inventory[n][m].itemm.equiptype<=ARTIFACT && map0.firsthero->equipment[map0.firsthero->inventory[n][m].itemm.equiptype].enabled==false)
                                    {
                                        map0.firsthero->equipment[map0.firsthero->inventory[n][m].itemm.equiptype].itemm=map0.firsthero->inventory[n][m].itemm;
                                        map0.firsthero->equipment[map0.firsthero->inventory[n][m].itemm.equiptype].enabled=true;
                                        map0.firsthero->inventory[n][m].enabled=false;
                                        break;
                                    }

                                }
                            }
                        }
                    }
                    if (open[SPELLS]==true)
                    {
                        if (mousestate.x>bttms[LINESKILLLEFT].x and mousestate.x<bttms[LINESKILLLEFT].x+50 and mousestate.y>bttms[LINESKILLLEFT].y and mousestate.y<bttms[LINESKILLLEFT].y+50 and mousestate.buttons & 1)
                        {
                            selectlineskill-=1;if (selectlineskill<0) selectlineskill=2;
                        }else
                        if (mousestate.x>bttms[LINESKILLRIGHT].x and mousestate.x<bttms[LINESKILLRIGHT].x+50 and mousestate.y>bttms[LINESKILLRIGHT].y and mousestate.y<bttms[LINESKILLRIGHT].y+50 and mousestate.buttons & 1)
                        {
                            selectlineskill+=1;if (selectlineskill>2) selectlineskill=0;
                        }

                        for (int i=0;i<numspells;i++)
                        {
                            if (spells[i].lineskill==selectlineskill and map0.firsthero->spellpoints>0)
                            {// upgrate spells
                                if (spells[i].row==0 and mousestate.x>windx[SPELLS]+100 and mousestate.x<windx[SPELLS]+115 and mousestate.y>windy[SPELLS]+340 and mousestate.y<windy[SPELLS]+355 and mousestate.buttons & 1)
                                { //main lineskill
                                    map0.firsthero->spellpoints-=1;map0.firsthero->spellslvl[i]+=1;
                                }
                                for (int j=0;j<2;j++)
                                    for (int g=0;g<3;g++)
                                    { //other spells
                                        if (spells[i].row==j*3+g+1 and mousestate.x>windx[SPELLS]+30+g*70 and mousestate.x<windx[SPELLS]+45+g*70 and mousestate.y>windy[SPELLS]+270-j*70 and mousestate.y<windy[SPELLS]+285-j*70 and mousestate.buttons & 1)
                                        {
                                            map0.firsthero->spellpoints-=1;map0.firsthero->spellslvl[i]+=1;
                                        }
                                    }
                            }

                            if (spells[i].lineskill==selectlineskill and spells[i].passive==0 and map0.firsthero->choosingarea==0)
                            {
                                for (int j=0;j<2;j++)//activating spells
                                {
                                    for (int g=0;g<3;g++)
                                    {
                                        if (spells[i].row==j*3+g+1 and mousestate.x>windx[SPELLS]+30+g*70 and mousestate.x<windx[SPELLS]+80+g*70 and mousestate.y>windy[SPELLS]+270-j*70 and mousestate.y<windy[SPELLS]+320-j*70 and (mousestate.y>windy[SPELLS]+285-j*70 or mousestate.x>windx[SPELLS]+45+g*70) and mousestate.buttons & 1)
                                        {
                                            spells[i].doingfunc(map0.firsthero,focusx,focusy,objects);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (open[INFO]==true)
                    {
                        for (int i=0;i<4;i++)
                        {
                            if (mousestate.x>windx[INFO]+140 and mousestate.x<windx[INFO]+155 and mousestate.y>windy[INFO]+215+16*i and mousestate.y<windy[INFO]+230+16*i and mousestate.buttons & 1)
                            {
                                if (map0.firsthero->attribpoints>0) {map0.firsthero->attributes[i]+=1;map0.firsthero->attribpoints-=1;}
                            }
                        }
                    }
                        for (monster *m=map0.firstmonster;m!=NULL;m=(monster*)m->next)
                            if (focusx==m->x and focusy==m->y) {selectperson=m;selectx=selectperson->x;selecty=selectperson->y;selecter=ENEMY;}
                        for (npc *n=map0.firstnpc;n!=NULL;n=(npc*)n->next)
                            if (focusx==n->x and focusy==n->y)
                            {
                                selectperson=n;selectx=selectperson->x;selecty=selectperson->y;selecter=NPC;
                            }

                    if (map0.firsthero->choosingarea==1 and (mousestate.x<windx[SPELLS] or mousestate.x>windx[SPELLS]+250 or mousestate.y<windy[SPELLS] or mousestate.y>windy[SPELLS]+400))
                        spells[map0.firsthero->choosedspell].doingfunc(map0.firsthero,focusx,focusy,objects);
                }

                if (state==MAINMENU)
                {
                    if ((mousestate.x>bttms[NEWGAME].x && mousestate.x<bttms[NEWGAME].x+bttms[NEWGAME].width && mousestate.y>bttms[NEWGAME].y && mousestate.y<bttms[NEWGAME].y+bttms[NEWGAME].height && bttms[NEWGAME].visible==true) && mousestate.buttons & 1)
                    {
                        for (int i=NEWGAME;i<=EXIT;i++)
                            bttms[i].visible=false;

                        state=GAME;
                    }

                    else if ((mousestate.x>bttms[EXIT].x && mousestate.x<bttms[EXIT].x+bttms[EXIT].width && mousestate.y>bttms[EXIT].y && mousestate.y<bttms[EXIT].y+bttms[EXIT].height && bttms[EXIT].visible==true) && mousestate.buttons & 1)
                    {
                        done=true;
                    }
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) //WINDOW CLOSE
        {
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER)  // TIMER
        {
            redraw = true;

            if (state==GAME)
            {
                for (object*o = map0.firstobj;o!=NULL;o=o->next)//object animation count
                {
                    if (o->animation==1) o->counter+=1;
                    if (o->num==3 and o->counter*o->nu==60)
                        for (monster*m = map0.firstmonster;(m!=NULL and map0.nummonsters!=0);m=(monster*)m->next)
                            if (o->x==m->x and o->y==m->y) {m->hp-=10*map0.firsthero->spellslvl[3];}
                    if (o->num==3 and o->counter*o->nu==120)
                    {
                        if (o==map0.firstobj)
                                {map0.firstobj=o->next;}
                        else
                        {
                            for (object*o0 = map0.firstobj;(o0!=NULL and map0.numobj!=0);o0=o0->next)
                            {
                                if (o0->next==o) o0->next=o->next;
                                if (&o->next==map0.pnewobj) map0.pnewobj=&o0->next;
                            }
                        }
                        map0.numobj-=1;
                    }

                    if (o->counter*o->nu>120) o->counter=0;
                }

                if (selectperson!=NULL) {selectx=selectperson->x;selecty=selectperson->y;}

                map0.firsthero->maxhp=(map0.firsthero->attributes[STRENGTH])*11;
                map0.firsthero->maxmp=(map0.firsthero->attributes[INTELLIGENCE])*10;
                map0.firsthero->getarmor();
                map0.firsthero->getdamage();
                map0.firsthero->sps=1.8;
                map0.firsthero->aps=2;
                map0.firsthero->hpregen=map0.firsthero->attributes[STRENGTH]*0.1;
                map0.firsthero->mpregen=map0.firsthero->attributes[INTELLIGENCE]*0.1;
                map0.firsthero->magicresist=25;

                for (int i=0;i<numspells;i++) //passive spells
                {
                    if (spells[i].passive==1 and spells[i].lineskill!=-1)
                    {
                        spells[i].doingfunc(map0.firsthero,focusx,focusy,objects);
                    }
                }

                if (map0.firsthero->alive==1) if (map0.firsthero->hpregencount*map0.firsthero->hpregen<60) map0.firsthero->hpregencount+=1;
                else {if (map0.firsthero->hp<map0.firsthero->maxhp) map0.firsthero->hp+=1;map0.firsthero->hpregencount=0;}
                else map0.firsthero->hpregen=0;
                if (map0.firsthero->alive==1) if (map0.firsthero->mpregencount*map0.firsthero->mpregen<60) map0.firsthero->mpregencount+=1;
                else {if (map0.firsthero->mp<map0.firsthero->maxmp) map0.firsthero->mp+=1;map0.firsthero->mpregencount=0;}
                else map0.firsthero->mpregen=0;

                if (map0.firsthero->exp>=map0.firsthero->needexp)//regeniration
                {
                    map0.firsthero->exp=fabsf(map0.firsthero->exp-map0.firsthero->needexp);
                    map0.firsthero->spellpoints+=1;
                    map0.firsthero->attribpoints+=1;
                }

                if (map0.firsthero->hp<=0)
                {
                    map0.firsthero->hp=0;
                    map0.firsthero->busy=true;map0.firsthero->deal=DEATH;
                    map0.firsthero->alive=false;
                }

                if (map0.firsthero->busy==false && (keys[LEFT]==true or keys[RIGHT]==true or keys[UP]==true or keys[DOWN]==true or mousestate.buttons & 1))
                {  //hero deal equally
                    if (keys[LEFT]==true) map0.firsthero->dir=LEFT; else if (keys[RIGHT]==true) map0.firsthero->dir=RIGHT;
                    else if (keys[UP]==true) map0.firsthero->dir=UP; else if (keys[DOWN]==true) map0.firsthero->dir=DOWN;

                    if (keys[LEFT]==true && map0.firsthero->x!=0 && keys[SPACE]==false && map0.checkway(*map0.firsthero,*map0.firstmonster) &&
                        map0.checkway(*map0.firsthero,*map0.firsthero) && map0.checkway(*map0.firsthero,*map0.firstnpc) && map0.checkway(*map0.firsthero,LEFT))
                            {map0.firsthero->busy=true;map0.firsthero->deal=WALKLEFT;map0.firsthero->dir=LEFT;}
                    else if (keys[RIGHT]==true && map0.firsthero->x!=map0.sizex-1 && keys[SPACE]==false && map0.checkway(*map0.firsthero,*map0.firstmonster) &&
                        map0.checkway(*map0.firsthero,*map0.firsthero) && map0.checkway(*map0.firsthero,*map0.firstnpc) && map0.checkway(*map0.firsthero,RIGHT))
                            {map0.firsthero->busy=true;map0.firsthero->deal=WALKRIGHT;map0.firsthero->dir=RIGHT;}
                    else if (keys[UP]==true && map0.firsthero->y!=0 && keys[SPACE]==false && map0.checkway(*map0.firsthero,*map0.firstmonster) &&
                        map0.checkway(*map0.firsthero,*map0.firsthero) && map0.checkway(*map0.firsthero,*map0.firstnpc) && map0.checkway(*map0.firsthero,UP))
                            {map0.firsthero->busy=true;map0.firsthero->deal=WALKUP;map0.firsthero->dir=UP;}
                    else if (keys[DOWN]==true && map0.firsthero->y!=map0.sizey-1 && keys[SPACE]==false && map0.checkway(*map0.firsthero,*map0.firstmonster) &&
                        map0.checkway(*map0.firsthero,*map0.firsthero) && map0.checkway(*map0.firsthero,*map0.firstnpc) && map0.checkway(*map0.firsthero,DOWN))
                            {map0.firsthero->busy=true;map0.firsthero->deal=WALKDOWN;map0.firsthero->dir=DOWN;}
                    else
                    {
                        for (monster*m = map0.firstmonster;m!=NULL;m=(monster*)m->next)
                        {
                           if (focusx==m->x && focusy==m->y &&
                                mousestate.buttons & 1 && m->alive==true &&
                            (focusx-map0.firsthero->x<=1 && focusx-map0.firsthero->x>=-1 && focusy==map0.firsthero->y) xor
                            (focusy-map0.firsthero->y<=1 && focusy-map0.firsthero->y>=-1 && focusx==map0.firsthero->x))
                            {
                                map0.firsthero->busy=true;map0.firsthero->deal=ATTACK;
                                if (focusx-map0.firsthero->x==1) map0.firsthero->dir=3;
                                else if (focusx-map0.firsthero->x==-1) map0.firsthero->dir=2;
                                else if (focusy-map0.firsthero->y==1) map0.firsthero->dir=1;
                                else map0.firsthero->dir=0;
                            }
                        }
                    }
                }
                {  //hero deal
                map0.firsthero->z=map0.coord[map0.firsthero->x][map0.firsthero->y].enterz+
                    (map0.firsthero->stpcnt*quasicos(map0.firsthero->dir)*
                     (2>translate(map0.coord[map0.firsthero->x][map0.firsthero->y].dir,map0.firsthero->dir))
                     +FPS/map0.firsthero->sps/2)/(FPS/map0.firsthero->sps/2)*
                     map0.coord[map0.firsthero->x][map0.firsthero->y].up/2;
                if (map0.firsthero->deal==WALKUP or map0.firsthero->deal==WALKDOWN or map0.firsthero->deal==WALKRIGHT or map0.firsthero->deal==WALKLEFT)
                {
                    map0.firsthero->stpcnt+=1;
                    map0.firsthero->z=map0.coord[map0.firsthero->x][map0.firsthero->y].enterz+
                    (map0.firsthero->stpcnt*quasicos(map0.firsthero->dir)*
                     (2>translate(map0.coord[map0.firsthero->x][map0.firsthero->y].dir,map0.firsthero->dir))
                     +FPS/map0.firsthero->sps/2)/(FPS/map0.firsthero->sps/2)*
                     map0.coord[map0.firsthero->x][map0.firsthero->y].up/2;
                    if (map0.firsthero->stpcnt >= FPS/map0.firsthero->sps/2)
                    {
                        switch (map0.firsthero->deal)
                        {
                        case 0:
                            map0.firsthero->y-=1;
                            break;
                        case 1:
                            map0.firsthero->y+=1;
                            break;
                        case 2:
                            map0.firsthero->x-=1;
                            break;
                        case 3:
                            map0.firsthero->x+=1;
                            break;
                        }
                        map0.firsthero->stpcnt=int(-FPS/map0.firsthero->sps/2);
                    }
                        if (map0.firsthero->stpcnt==0)
                        {
                            map0.firsthero->deal=NOTHING;
                            map0.firsthero->busy=false;
                            map0.firsthero->stpcnt=0;
                        }
                }
                else if (map0.firsthero->deal==ATTACK)
                {
                    map0.firsthero->attckcnt+=1;
                    if (map0.firsthero->attckcnt >= FPS/ map0.firsthero->aps)
                    {
                        for (monster*m = map0.firstmonster;m!=NULL;m=(monster*)m->next)
                        {
                            switch (map0.firsthero->dir)
                            {
                            case 0:
                                if (m->x-map0.firsthero->x==0 && m->y-map0.firsthero->y==-1)
                                    m->hp-=map0.firsthero->damage-map0.firsthero->damage*(m->armor*0.06/(1+m->armor*0.06));
                                    break;
                            case 1:
                                if (m->x-map0.firsthero->x==0 && m->y-map0.firsthero->y==1)
                                    m->hp-=map0.firsthero->damage-map0.firsthero->damage*(m->armor*0.06/(1+m->armor*0.06));
                                    break;
                            case 2:
                                if (m->x-map0.firsthero->x==-1 && m->y-map0.firsthero->y==0)
                                    m->hp-=map0.firsthero->damage-map0.firsthero->damage*(m->armor*0.06/(1+m->armor*0.06));
                                    break;
                            case 3:
                                if (m->x-map0.firsthero->x==1 && m->y-map0.firsthero->y==0)
                                    m->hp-=map0.firsthero->damage-map0.firsthero->damage*(m->armor*0.06/(1+m->armor*0.06));
                                    break;
                            }
                            if (m->hp<=0 && m->alive==true) map0.firsthero->exp+=19;
                        }
                        map0.firsthero->deal=NOTHING;
                        map0.firsthero->busy=false;
                        map0.firsthero->attckcnt=0;
                    }
                }
                }

                for (monster*m = map0.firstmonster;(m!=NULL and map0.nummonsters!=0);m=(monster*)m->next) //monsters
                {
                    m->z=map0.coord[m->x][m->y].enterz+(m->stpcnt*quasicos(m->dir)*
                        (2>translate(map0.coord[m->x][m->y].dir,m->dir))
                        +FPS/m->sps/2)/(FPS/m->sps/2)*map0.coord[m->x][m->y].up/2;
                    if (m->hp<=0 && m->deal!=DEATH)
                    {
                        m->alive=false;m->busy=true;m->deal=DEATH;
                    }
                    if (m->busy==false)
                    {  //monster deal equally
                        if (abs(map0.firsthero->x-m->x)<=m->agrrange && abs(map0.firsthero->y-m->y)<=m->agrrange)
                        {m->dir = UP;//vieving
                        if (m->dir==UP && m->y!=0 && map0.checkway(*m,*map0.firsthero) &&
                            map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                            map0.checkway(*m,UP))
                        {
                            m->landmind[m->x][m->y-1]=0;
                        }
                        m->dir=DOWN;
                        if (m->dir==DOWN && m->y!=map0.sizey-1 && map0.checkway(*m,*map0.firsthero) &&
                            map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                            map0.checkway(*m,DOWN))
                        {
                            m->landmind[m->x][m->y+1]=0;
                        }
                        m->dir=LEFT;
                        if (m->dir==LEFT && m->x!=0 && map0.checkway(*m,*map0.firsthero) &&
                            map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                            map0.checkway(*m,LEFT))
                        {
                            m->landmind[m->x-1][m->y]=0;
                        }
                        m->dir=RIGHT;
                        if (m->dir==RIGHT && m->x!=map0.sizex-1 && map0.checkway(*m,*map0.firsthero) &&
                            map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                            map0.checkway(*m,RIGHT))
                        {
                            m->landmind[m->x+1][m->y]=0;
                        }
                        m->deal=walkatob(m->landmind,m->x,m->y,map0.firsthero->x,map0.firsthero->y);
                        if (m->deal==-1)
                            switch (rand()%4)
                            {
                            case 0:
                                m->deal=walkatob(m->landmind,m->x,m->y,map0.firsthero->x,map0.firsthero->y-1);
                                break;
                            case 1:
                                m->deal=walkatob(m->landmind,m->x,m->y,map0.firsthero->x,map0.firsthero->y+1);
                                break;
                            case 2:
                                m->deal=walkatob(m->landmind,m->x,m->y,map0.firsthero->x-1,map0.firsthero->y);
                                break;
                            case 3:
                                m->deal=walkatob(m->landmind,m->x,m->y,map0.firsthero->x+1,map0.firsthero->y);
                                break;
                            }
                        }
                        else
                        {
                            m->deal=rand()%4;
                        }

                        if (m->deal>=0 && m->deal<=3) m->dir=m->deal;
                        if (m->dir==UP && m->y!=0 && map0.checkway(*m,*map0.firsthero) &&
                            map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                            map0.checkway(*m,UP))
                        {
                            m->landmind[m->x][m->y-1]=0;m->dir=UP;m->busy=true;
                        }
                        else if (m->dir==DOWN && m->y!=map0.sizey-1 && map0.checkway(*m,*map0.firsthero) &&
                                 map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                                map0.checkway(*m,DOWN))
                        {
                            m->landmind[m->x][m->y+1]=0;m->dir=DOWN;m->busy=true;
                        }
                        else if (m->dir==LEFT && m->x!=0 && map0.checkway(*m,*map0.firsthero) &&
                                 map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                                map0.checkway(*m,LEFT))
                        {
                            m->landmind[m->x-1][m->y]=0;m->dir=LEFT;m->busy=true;
                        }
                        else if (m->dir==RIGHT && m->x!=map0.sizex-1 && map0.checkway(*m,*map0.firsthero) &&
                                 map0.checkway(*m,*map0.firstmonster) && map0.checkway(*m,*map0.firstnpc) &&
                                map0.checkway(*m,RIGHT))
                        {
                            m->landmind[m->x+1][m->y]=0;m->dir=RIGHT;m->busy=true;
                        }
                        else
                        {
                            switch (m->deal)
                            {
                            case 0:
                                m->landmind[m->x][m->y-1]=-1;
                                break;
                            case 1:
                                m->landmind[m->x][m->y+1]=-1;
                                break;
                            case 2:
                                m->landmind[m->x-1][m->y]=-1;
                                break;
                            case 3:
                                m->landmind[m->x+1][m->y]=-1;
                                break;
                            }
                            m->deal=NOTHING;
                        }
                        //start attack monster if near
                        if (map0.firsthero->x-1==m->x and map0.firsthero->y==m->y and map0.firsthero->z>=m->z-8 and map0.firsthero->z<=m->z+8)
                        {
                            m->dir=RIGHT;m->deal=ATTACK;m->busy=true;
                        }
                        else if (map0.firsthero->x+1==m->x and map0.firsthero->y==m->y and map0.firsthero->z>=m->z-8 and map0.firsthero->z<=m->z+8)
                        {
                            m->dir=LEFT;m->deal=ATTACK;m->busy=true;
                        }
                        else if (map0.firsthero->x==m->x and map0.firsthero->y-1==m->y and map0.firsthero->z>=m->z-8 and map0.firsthero->z<=m->z+8)
                        {
                            m->dir=DOWN;m->deal=ATTACK;m->busy=true;
                        }
                        else if (map0.firsthero->x==m->x and map0.firsthero->y+1==m->y and map0.firsthero->z>=m->z-8 and map0.firsthero->z<=m->z+8)
                        {
                            m->dir=UP;m->deal=ATTACK;m->busy=true;
                        }
                    }

                    if (m->deal==WALKUP or m->deal==WALKDOWN or m->deal==WALKRIGHT or m->deal==WALKLEFT)
                    {  //monster deal
                        m->stpcnt+=1;
                        m->z=map0.coord[m->x][m->y].enterz+(m->stpcnt*quasicos(m->dir)*
                        (2>translate(map0.coord[m->x][m->y].dir,m->dir))
                        +FPS/m->sps/2)/(FPS/m->sps/2)*map0.coord[m->x][m->y].up/2;
                        if (m->stpcnt >=FPS/m->sps/2)
                        {
                            switch (m->deal)
                            {
                            case 0:
                                m->y-=1;
                                break;
                            case 1:
                                m->y+=1;
                                break;
                            case 2:
                                m->x-=1;
                                break;
                            case 3:
                                m->x+=1;
                                break;
                            }
                            m->stpcnt=int(-FPS/m->sps/2);
                        }
                        if (m->stpcnt==0)
                        {
                            m->deal=NOTHING;
                            m->busy=false;
                            m->stpcnt=0;
                        }
                    }
                    else if (m->deal==ATTACK)
                    {
                        m->attckcnt+=1;
                        if (m->attckcnt >= FPS/m->aps)
                        {
                            for (int d = 0;d<map0.numplayers;d++)
                            {
                                switch (m->dir)
                                {
                                case 0:
                                    if (-m->x+map0.firsthero->x==0 && -m->y+map0.firsthero->y==-1 && map0.firsthero->hp>0)
                                        map0.firsthero->hp-=m->damage-m->damage*(map0.firsthero->armor*0.06/(1+map0.firsthero->armor*0.06));
                                        break;
                                case 1:
                                    if (-m->x+map0.firsthero->x==0 && -m->y+map0.firsthero->y==1 && map0.firsthero->hp>0)
                                        map0.firsthero->hp-=m->damage-m->damage*(map0.firsthero->armor*0.06/(1+map0.firsthero->armor*0.06));
                                        break;
                                case 2:
                                    if (-m->x+map0.firsthero->x==-1 && -m->y+map0.firsthero->y==0 && map0.firsthero->hp>0)
                                        map0.firsthero->hp-=m->damage-m->damage*(map0.firsthero->armor*0.06/(1+map0.firsthero->armor*0.06));
                                        break;
                                case 3:
                                    if (-m->x+map0.firsthero->x==1 && -m->y+map0.firsthero->y==0 && map0.firsthero->hp>0)
                                        map0.firsthero->hp-=m->damage-m->damage*(map0.firsthero->armor*0.06/(1+map0.firsthero->armor*0.06));
                                        break;
                                }
                            }
                            m->deal=NOTHING;
                            m->busy=false;
                            m->attckcnt=0;
                        }
                    }
                    else if (m->deal==DEATH)
                    {
                        m->dthcnt++;
                        if (m->dthcnt==600)
                        {
                            if (m==map0.firstmonster)
                                {map0.firstmonster=(monster*)m->next;}
                            else
                            {
                                for (monster*m0 = map0.firstmonster;(m0!=NULL and map0.nummonsters!=0);m0=(monster*)m0->next)
                                    if (m0->next==m) m0->next=m->next;
                            }
                            map0.nummonsters-=1;
                        }
                    }
                }
            }
            else if (state==MAINMENU)
            {

            }
        }

        if (redraw && al_event_queue_is_empty(event_queue))//DRAWING
        {

            redraw = false;

            map0.focusing(&focusx,&focusy,*map0.firsthero,width,height,zoom,mousestate,camdir);

            frames++;
            if (al_current_time() - gameTime >=1) // FPS count
            {
                gameTime = int(al_current_time());
                gameFPS = frames;
                frames = 0;
            }

            if (state==GAME)
            {
                al_clear_to_color(al_map_rgb(0,0,0));
                al_draw_scaled_bitmap(floorimage[camdir], //draw floor
                                      -((-map0.firsthero->x+(2*map0.firsthero->x-map0.sizex+1)*(camdir%3!=0)-map0.firsthero->y+(2*map0.firsthero->y-map0.sizey+1)*(camdir>=2)-1)*16
                                        +width/2+M1_POW(translate((4-camdir)%4,map0.firsthero->dir))*map0.firsthero->stpcnt/2*map0.firsthero->sps*0.533),
                                      -((-map0.firsthero->x+(2*map0.firsthero->x-map0.sizex+1)*(camdir<=1)-map0.firsthero->y+(2*map0.firsthero->y-map0.sizey+1)*(camdir%3!=0))*8
                                        +height/2+quasicos(translate((4-camdir)%4,map0.firsthero->dir))*map0.firsthero->stpcnt/2*map0.firsthero->sps*0.266+map0.firsthero->z),
                                      width,height,-width*(float(zoom-1)/2),-height*(float(zoom-1)/2)+(zoom-1)*(8),
                                      width*zoom,height*zoom,0);
                for (int g=0;g<2;g++)//MAP AND HERO DRAWING
                {
                    for (int i=(camdir<2)?(map0.sizex-1):(0);(camdir<2)?(i>=0):(i<map0.sizex);(camdir<2)?(i--):(i++))
                    {
                        for (int j=(camdir%3==0)?(0):(map0.sizey-1);(camdir%3==0)?(j<map0.sizey):(j>=0);(camdir%3==0)?(j++):(j--))
                        {
                            if (g==1)
                            {
                                if (//block
                                        (i+j+width/(32*zoom)+3+map0.coord[i][j].height>map0.firsthero->x+map0.firsthero->y and
                                        j+i-width/(32*zoom)-3-map0.coord[i][j].height<map0.firsthero->x+map0.firsthero->y and
                                        i-j-height/(16*zoom)-4-map0.coord[i][j].height<map0.firsthero->x-map0.firsthero->y and
                                        j-i-height/(16*zoom)-3-map0.coord[i][j].height<map0.firsthero->y-map0.firsthero->x))
                                            map0.drawblock(i,j,*map0.firsthero,width,height,blocksimage,objimage,zoom,focusx,focusy,camdir);
                                if (i==focusx and j==focusy) //picker
                                    al_draw_scaled_bitmap((map0.firsthero->choosingarea)?areaselecter[0]:picker,0,0,32,16,
                                        (focusx+focusy-map0.firsthero->x-map0.firsthero->y-1-(camdir>=2)*2*(focusy-map0.firsthero->y)-(camdir%3!=0)*2*(focusx-map0.firsthero->x))*16*zoom+width/2+
                                        M1_POW(translate((4-camdir)%4,map0.firsthero->dir))*map0.firsthero->stpcnt/2*map0.firsthero->sps*zoom*0.54,
                                        -8+(focusx+focusy-map0.firsthero->x-map0.firsthero->y+1-(camdir<=1)*2*(focusx-map0.firsthero->x)-(camdir%3!=0)*2*(focusy-map0.firsthero->y))*8*zoom+height/2+
                                        quasicos(translate((4-camdir)%4,map0.firsthero->dir))*map0.firsthero->stpcnt/2*map0.firsthero->sps*zoom*0.28+(map0.firsthero->z-map0.coord[focusx][focusy].enterz-map0.coord[focusx][focusy].up)*zoom,
                                        32*zoom,16*zoom,0);
                                if (i==selectx and j==selecty and selectperson!=NULL) //selecter
                                    al_draw_scaled_bitmap(selecterimg[selecter],0,0,32,16,
                                        (selectx+selecty-map0.firsthero->x-map0.firsthero->y-1-(camdir>=2)*2*(selecty-map0.firsthero->y)-(camdir%3!=0)*2*(selectx-map0.firsthero->x))*16*zoom+width/2+
                                        M1_POW(translate((4-camdir)%4,map0.firsthero->dir))*map0.firsthero->stpcnt/2*map0.firsthero->sps*zoom*0.54,
                                        -8+(selectx+selecty-map0.firsthero->x-map0.firsthero->y+1-(camdir<=1)*2*(selectx-map0.firsthero->x)-(camdir%3!=0)*2*(selecty-map0.firsthero->y))*8*zoom+height/2+
                                        quasicos(translate((4-camdir)%4,map0.firsthero->dir))*map0.firsthero->stpcnt/2*map0.firsthero->sps*zoom*0.28+(map0.firsthero->z-map0.coord[selectx][selecty].enterz-map0.coord[selectx][selecty].up)*zoom,
                                        32*zoom,16*zoom,0);
                                for (monster*m = map0.firstmonster;(m!=NULL and map0.nummonsters!=0);m=(monster*)m->next)//monsters
                                {
                                    if (m->x==i and m->y==j)
                                    {
                                        m->draw(i,j,*map0.firsthero,width,height,monsterimage[m->num],healthbar,barhealth,zoom,camdir);
                                    }
                                }

                                for (npc*n = map0.firstnpc;n!=NULL;n=(npc*)n->next) //npc
                                {
                                    if (n->x==i and n->y==j)
                                    {
                                        n->draw(i,j,*map0.firsthero,width,height,zoom,camdir);
                                    }
                                }

                                if (i==map0.firsthero->x and j==map0.firsthero->y)//hero
                                    map0.firsthero->draw(height,width,zoom,camdir);
                            }
                        }
                    }
                }
                if (map0.firsthero->alive==false) al_draw_bitmap(gameoverimage,width/2-150,height/2-100,0);

                for (int i = BAG;i<=QUESTBOOK;i++) //BUTTONS TABS
                {
                    if (bttms[i].visible==true)
                    {
                        if (open[i]==false)//tabs windows
                        {
                            if (mousestate.x>bttms[i].x and mousestate.x<bttms[i].x+bttms[i].width and mousestate.y>bttms[i].y and mousestate.y<bttms[i].y+bttms[i].height)
                                bttms[i].x=0;
                            else if (bttms[i].x==0)
                                bttms[i].x=-9;
                            bttms[i].y=225+50*i;
                            al_draw_bitmap(bttms[i].image[1],bttms[i].x,bttms[i].y,0);//tabs
                            al_draw_bitmap(bttms[i].image[0],bttms[i].x,bttms[i].y,0);
                            if (i==BAG and open[BAG]==true) al_draw_bitmap(closeimage,bttms[BAG].x+9,bttms[BAG].y+9,0);
                            if (i==SPELLS and open[SPELLS]==true) al_draw_bitmap(closeimage,bttms[SPELLS].x+9,bttms[SPELLS].y+9,0);
                            else if (i==SPELLS and map0.firsthero->spellpoints>0) al_draw_bitmap(imgplus,bttms[i].x+35,bttms[i].y,0);
                            if (i==INFO and open[INFO]==true) al_draw_bitmap(closeimage,bttms[INFO].x+9,bttms[INFO].y+9,0);
                            else if (i==INFO and map0.firsthero->attribpoints>0) al_draw_bitmap(imgplus,bttms[i].x+35,bttms[i].y,0);
                            if (i==QUESTBOOK and open[QUESTBOOK]==true) al_draw_bitmap(closeimage,bttms[QUESTBOOK].x+9,bttms[QUESTBOOK].y+9,0);
                        }
                        else
                        {
                            al_draw_bitmap(pageimage,windx[i],windy[i],0);
                            bttms[i].x=windx[i]+248;bttms[i].y=windy[i]+5;

                            al_draw_bitmap(bttms[i].image[1],bttms[i].x,bttms[i].y,0);//tabs
                            al_draw_bitmap(bttms[i].image[0],bttms[i].x,bttms[i].y,0);
                            if (i==BAG) al_draw_bitmap(closeimage,bttms[BAG].x+9,bttms[BAG].y+9,0);
                            if (i==SPELLS) al_draw_bitmap(closeimage,bttms[SPELLS].x+9,bttms[SPELLS].y+9,0);
                            if (i==INFO) al_draw_bitmap(closeimage,bttms[INFO].x+9,bttms[INFO].y+9,0);
                            if (i==QUESTBOOK) al_draw_bitmap(closeimage,bttms[QUESTBOOK].x+9,bttms[QUESTBOOK].y+9,0);

                            if (i==BAG) //BAG
                            {
                                al_draw_bitmap(coinimage,windx[BAG]+10,windy[BAG]+224,0);
                                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),windx[BAG]+32,windy[BAG]+228,ALLEGRO_ALIGN_LEFT,
                                    "%hd",map0.firsthero->money);

                                al_draw_bitmap(playerframe,windx[BAG]+72,windy[BAG]+15,0);
                                if ((map0.firsthero->dir==UP or map0.firsthero->dir==RIGHT) and map0.firsthero->equipment[WEAPON].enabled) al_draw_scaled_bitmap(map0.firsthero->image[WEAPONIMAGE],0,map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:48,32,48,windx[BAG]+122-16*4,windy[BAG]-24*4+94,32*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                al_draw_scaled_bitmap(map0.firsthero->image[BODY],0,map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:48,24,48,windx[BAG]+122-12*4,windy[BAG]-24*4+94,24*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                if (map0.firsthero->equipment[GLOVES].enabled) al_draw_scaled_bitmap(map0.firsthero->image[ARM],0,map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0+96:48,24,48,windx[BAG]+122-12*4,windy[BAG]-24*4+94,24*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                if (map0.firsthero->equipment[LEGGINGS].enabled) al_draw_scaled_bitmap(map0.firsthero->image[LEGS],map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:24,0,24,48,windx[BAG]+122-12*4,windy[BAG]-24*4+94,24*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                if (map0.firsthero->equipment[HELM].enabled) al_draw_scaled_bitmap(map0.firsthero->image[HEAD],map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:24,0,24,48,windx[BAG]+122-12*4,windy[BAG]-24*4+94,24*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                if (map0.firsthero->equipment[ARMOR].enabled) al_draw_scaled_bitmap(map0.firsthero->image[TRUNK],map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:24,0,24,48,windx[BAG]+122-12*4,windy[BAG]-24*4+94,24*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                if (map0.firsthero->equipment[GLOVES].enabled) al_draw_scaled_bitmap(map0.firsthero->image[ARM],0,map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:48+96,24,48,windx[BAG]+122-12*4,windy[BAG]-24*4+94,24*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);
                                if ((map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT) and map0.firsthero->equipment[WEAPON].enabled) al_draw_scaled_bitmap(map0.firsthero->image[WEAPONIMAGE],0,map0.firsthero->dir==DOWN or map0.firsthero->dir==LEFT?0:48,32,48,windx[BAG]+122-16*4,windy[BAG]-24*4+94,32*4,48*4,map0.firsthero->dir==LEFT or map0.firsthero->dir==RIGHT?1:0);

                                for (int m=0;m<2;m++) //draw slots for equiping and equip
                                {
                                    for(int n=0;n<4;n++)
                                    {
                                        al_draw_bitmap(spllfrm,windx[BAG]+10+m*170,windy[BAG]+6+n*55,0);
                                        if (map0.firsthero->equipment[m*4+n].enabled==true) al_draw_bitmap(map0.firsthero->equipment[m*4+n].itemm.icon,windx[BAG]+10+m*170,windy[BAG]+6+n*55,0);
                                    }
                                }

                                for (int m=0;m<2;m++) //draw enteractwind for equip
                                {
                                    for(int n=0;n<4;n++)
                                    {
                                        if (mousestate.x>windx[BAG]+10+m*170 and mousestate.x<windx[BAG]+60+m*170 and mousestate.y>windy[BAG]+6+n*55 and mousestate.y<windy[BAG]+56+n*55 and map0.firsthero->equipment[m*4+n].enabled==1)
                                        {
                                            al_draw_bitmap(enteractwind,mousestate.x,mousestate.y-150,0);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-135,ALLEGRO_ALIGN_LEFT,"%s",map0.firsthero->equipment[m*4+n].itemm.name);
                                            if (map0.firsthero->equipment[m*4+n].itemm.damage!=0)
                                            {al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-115,ALLEGRO_ALIGN_LEFT,"damage %d",map0.firsthero->equipment[m*4+n].itemm.damage);
                                            if (map0.firsthero->equipment[m*4+n].itemm.armor!=0)
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-100,ALLEGRO_ALIGN_LEFT,"armor %d",map0.firsthero->equipment[m*4+n].itemm.armor);}
                                            else if (map0.firsthero->equipment[m*4+n].itemm.armor!=0)
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-115,ALLEGRO_ALIGN_LEFT,"armor %d",map0.firsthero->equipment[m*4+n].itemm.armor);
                                            al_draw_bitmap(coinimage,mousestate.x+140,mousestate.y-35,0);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+160,mousestate.y-32,ALLEGRO_ALIGN_LEFT,"%d",map0.firsthero->equipment[m*4+n].itemm.cost);
                                        }
                                    }
                                }

                                for (int m=0;m<3;m++) //draw slots
                                {
                                    for (int n=0;n<4;n++)
                                    {
                                        al_draw_bitmap(spllfrm,windx[BAG]+4+n*50,windy[BAG]+246+m*50,0);
                                        if (map0.firsthero->inventory[n][m].enabled==true) al_draw_bitmap(map0.firsthero->inventory[n][m].itemm.icon,windx[BAG]+4+n*50,windy[BAG]+246+m*50,0);
                                    }
                                }

                                for (int m=0;m<3;m++) //draw enteractwind for slots
                                {
                                    for (int n=0;n<4;n++)
                                    {
                                        if (mousestate.x>windx[BAG]+4+n*50 and mousestate.x<windx[BAG]+54+n*50 and mousestate.y>windy[BAG]+246+m*50 and mousestate.y<windy[BAG]+296+m*50 and map0.firsthero->inventory[n][m].enabled==1)
                                        {
                                            al_draw_bitmap(enteractwind,mousestate.x,mousestate.y-150,0);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-135,ALLEGRO_ALIGN_LEFT,"%s",map0.firsthero->inventory[n][m].itemm.name);
                                            if (map0.firsthero->inventory[n][m].itemm.damage!=0)
                                            {al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-115,ALLEGRO_ALIGN_LEFT,"damage %d",map0.firsthero->inventory[n][m].itemm.damage);
                                            if (map0.firsthero->inventory[n][m].itemm.armor!=0)
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-100,ALLEGRO_ALIGN_LEFT,"armor %d",map0.firsthero->inventory[n][m].itemm.armor);}
                                            else if (map0.firsthero->inventory[n][m].itemm.armor!=0)
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-115,ALLEGRO_ALIGN_LEFT,"armor %d",map0.firsthero->inventory[n][m].itemm.armor);
                                            al_draw_bitmap(coinimage,mousestate.x+140,mousestate.y-35,0);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+160,mousestate.y-32,ALLEGRO_ALIGN_LEFT,"%d",map0.firsthero->inventory[n][m].itemm.cost);
                                        }

                                    }
                                }
                            }
                            else if (i==SPELLS) //SPELLS
                            {
                                al_draw_textf(pixelcyrmenu,al_map_rgb(0,0,0),windx[SPELLS]+225,windy[SPELLS]+370,
                                              ALLEGRO_ALIGN_RIGHT,"%i",map0.firsthero->spellpoints);
                                al_draw_bitmap(imgplus,windx[SPELLS]+227,windy[SPELLS]+374,0);


                                for (int d=0;d<numspells;d++)
                                    if (spells[d].lineskill==selectlineskill and spells[d].row==0)
                                        {
                                            al_draw_bitmap(spllfrm,windx[SPELLS]+100,windy[SPELLS]+340,0);
                                            al_draw_bitmap(spells[d].icon,windx[SPELLS]+103,windy[SPELLS]+343,0);
                                            if (map0.firsthero->spellpoints!=0) al_draw_bitmap(imgplus,
                                                                windx[SPELLS]+100,windy[SPELLS]+340,0);
                                            al_draw_filled_rectangle(windx[SPELLS]+138,windy[SPELLS]+378,windx[SPELLS]+150,windy[SPELLS]+390,al_map_rgb(255,255,255));
                                            al_draw_rectangle(windx[SPELLS]+138,windy[SPELLS]+378,windx[SPELLS]+150,windy[SPELLS]+390,al_map_rgb(0,0,0),1);
                                            al_draw_textf(pixelcyr,al_map_rgb(0,0,0),windx[SPELLS]+141,windy[SPELLS]+376,ALLEGRO_ALIGN_LEFT,"%i",map0.firsthero->spellslvl[d]);
                                        }

                                bttms[LINESKILLLEFT].x=windx[SPELLS]+48;bttms[LINESKILLRIGHT].x=windx[SPELLS]+152;
                                bttms[LINESKILLLEFT].y=bttms[LINESKILLRIGHT].y=windy[SPELLS]+340;
                                al_draw_bitmap(bttms[LINESKILLLEFT].image[0],bttms[LINESKILLLEFT].x,bttms[LINESKILLLEFT].y,0);
                                al_draw_bitmap(bttms[LINESKILLRIGHT].image[0],bttms[LINESKILLRIGHT].x,bttms[LINESKILLRIGHT].y,1);

                                for (int j=0;j<2;j++) //spells icons
                                    for (int g=0;g<3;g++)
                                    {
                                        for (int d=0;d<numspells;d++)
                                        if (spells[d].row==j*3+g+1 and spells[d].lineskill==selectlineskill)
                                        {
                                            al_draw_bitmap(spllfrm,windx[SPELLS]+30+g*70,windy[SPELLS]+270-j*70,0);
                                            if (map0.firsthero->spellslvl[d]==0)
                                                al_draw_tinted_bitmap(spells[d].icon,al_map_rgb(128,128,128),
                                                                  windx[SPELLS]+33+g*70,windy[SPELLS]+273-j*70,0);
                                            else al_draw_bitmap(spells[d].icon,windx[SPELLS]+33+g*70,windy[SPELLS]+273-j*70,0);

                                            if (map0.firsthero->spellpoints!=0) al_draw_bitmap(imgplus,
                                                                windx[SPELLS]+30+g*70,windy[SPELLS]+270-j*70,0);

                                            al_draw_filled_rectangle(windx[SPELLS]+68+g*70,windy[SPELLS]+308-j*70,windx[SPELLS]+80+g*70,windy[SPELLS]+320-j*70,al_map_rgb(255,255,255));
                                            al_draw_rectangle(windx[SPELLS]+68+g*70,windy[SPELLS]+308-j*70,windx[SPELLS]+80+g*70,windy[SPELLS]+320-j*70,al_map_rgb(0,0,0),1);
                                            al_draw_textf(pixelcyr,al_map_rgb(0,0,0),windx[SPELLS]+71+g*70,windy[SPELLS]+306-j*70,ALLEGRO_ALIGN_LEFT,"%i",map0.firsthero->spellslvl[d]);
                                        }
                                    }
                                for (int j=0;j<2;j++) //description bar for spells
                                    for (int g=0;g<3;g++)
                                    {
                                        for (int d=0;d<numspells;d++)
                                        if (spells[d].row==j*3+g+1 and spells[d].lineskill==selectlineskill and mousestate.x>windx[SPELLS]+30+g*70 and mousestate.x<windx[SPELLS]+80+g*70 and mousestate.y>windy[SPELLS]+270-j*70 and mousestate.y<windy[SPELLS]+320-j*70)
                                        {
                                            al_draw_bitmap(enteractwind,mousestate.x,mousestate.y-150,0);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-135,ALLEGRO_ALIGN_LEFT,"%s",spells[d].name);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-120,ALLEGRO_ALIGN_LEFT,"%s",spells[d].description);
                                        }else
                                        if (spells[d].row==0 and spells[d].lineskill==selectlineskill and mousestate.x>windx[SPELLS]+100 and mousestate.x<windx[SPELLS]+150 and mousestate.y>windy[SPELLS]+340 and mousestate.y<windy[SPELLS]+390)
                                        {
                                            al_draw_bitmap(enteractwind,mousestate.x,mousestate.y-150,0);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-135,ALLEGRO_ALIGN_LEFT,"%s",spells[d].name);
                                            al_draw_textf(calibri,al_map_rgb(0,0,0),mousestate.x+15,mousestate.y-120,ALLEGRO_ALIGN_LEFT,"%s",spells[d].description);
                                        }
                                    }

                            }
                            else if (i==INFO)
                            {
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+10,
                                              ALLEGRO_ALIGN_LEFT,"Max health points=%d",(int)map0.firsthero->maxhp);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+25,
                                              ALLEGRO_ALIGN_LEFT,"Max mana points=%d",(int)map0.firsthero->maxmp);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+40,
                                              ALLEGRO_ALIGN_LEFT,"Hp regeniration=%.1f",map0.firsthero->hpregen);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+55,
                                              ALLEGRO_ALIGN_LEFT,"Mp regeniration=%.1f",map0.firsthero->mpregen);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+70,
                                              ALLEGRO_ALIGN_LEFT,"Armor=%i",map0.firsthero->armor);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+85,
                                              ALLEGRO_ALIGN_LEFT,"Magic resist=%i",map0.firsthero->magicresist);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+100,
                                              ALLEGRO_ALIGN_LEFT,"Damage=%i",map0.firsthero->damage);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+115,
                                              ALLEGRO_ALIGN_LEFT,"Attacks per second=%.1f",map0.firsthero->aps);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+130,
                                              ALLEGRO_ALIGN_LEFT,"Move speed=%0.1f",map0.firsthero->sps);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+145,
                                              ALLEGRO_ALIGN_LEFT,"Level=%i",map0.firsthero->lvl);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+160,
                                              ALLEGRO_ALIGN_LEFT,"Experience for next lvl=%i",(int)map0.firsthero->needexp);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+175,
                                              ALLEGRO_ALIGN_LEFT,"Experience=%i",(int)map0.firsthero->exp);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+200,
                                              ALLEGRO_ALIGN_LEFT,"Attributes:             Not used %i",map0.firsthero->attribpoints);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+215,
                                              ALLEGRO_ALIGN_LEFT,"Strength                      %i",map0.firsthero->attributes[STRENGTH]);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+230,
                                              ALLEGRO_ALIGN_LEFT,"Agility                          %i",map0.firsthero->attributes[AGILITY]);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+245,
                                              ALLEGRO_ALIGN_LEFT,"Intelligence                %i",map0.firsthero->attributes[INTELLIGENCE]);
                                al_draw_textf(calibri,al_map_rgb(0,0,0),windx[INFO]+10,windy[INFO]+260,
                                              ALLEGRO_ALIGN_LEFT,"Lucky                           %i",map0.firsthero->attributes[LUCKY]);
                                if (map0.firsthero->attribpoints>0)
                                {
                                    al_draw_bitmap(imgplus,windx[INFO]+140,windy[INFO]+215,0);
                                    al_draw_bitmap(imgplus,windx[INFO]+140,windy[INFO]+231,0);
                                    al_draw_bitmap(imgplus,windx[INFO]+140,windy[INFO]+247,0);
                                    al_draw_bitmap(imgplus,windx[INFO]+140,windy[INFO]+263,0);
                                }

                            }
                        }
                    }
                }

                al_draw_bitmap(minimap,width-100,0,0); //MINI MAP
                for (int j = 0;j<map0.sizey;j++)
                {
                    for (int i = 0;i<map0.sizex;i++)
                    {
                        al_draw_pixel(width-84+i,6+j,map0.coord[i][j].color);
                        if (i==map0.firsthero->x and j==map0.firsthero->y) al_draw_pixel(width-84+map0.firsthero->x,6+map0.firsthero->y,al_map_rgb(255,255,0));
                        for (monster*m = map0.firstmonster;m!=NULL;m=(monster*)m->next)
                            if (i==m->x and j==m->y)
                                al_draw_pixel(width-84+m->x,6+m->y,al_map_rgb(255,0,0));
                    }
                }

                {al_draw_bitmap(statusbar,0,0,0); //Status Bar
                al_draw_bitmap(classicons,5,5,0);

                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),70,5,ALLEGRO_ALIGN_LEFT,
                                "Level:%d",map0.firsthero->lvl);
                al_draw_bitmap(attackindexsword,123,5,0);
                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),140,5,ALLEGRO_ALIGN_LEFT,
                                "%d",map0.firsthero->damage);
                al_draw_bitmap(armorindex,176,5,0);
                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),192,5,ALLEGRO_ALIGN_LEFT,
                                "%d",map0.firsthero->armor);

                al_draw_bitmap(heart,74,24,0);
                al_draw_bitmap_region(health,
                                      0,36-(map0.firsthero->hp/ map0.firsthero->maxhp)*36,
                                      36,(map0.firsthero->hp/ map0.firsthero->maxhp)*36,
                                      77,27+36-(map0.firsthero->hp/ map0.firsthero->maxhp)*36,0);
                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),95,65,ALLEGRO_ALIGN_CENTER,
                                "%d/%d",int(map0.firsthero->hp),int(map0.firsthero->maxhp));

                al_draw_bitmap(bottle,127,24,0);
                al_draw_bitmap_region(mana,
                                      0,36-(map0.firsthero->mp/ map0.firsthero->maxmp)*36,
                                      36,(map0.firsthero->mp/ map0.firsthero->maxmp)*36,
                                      130,27+36-(map0.firsthero->mp/ map0.firsthero->maxmp)*36,0);
                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),148,65,ALLEGRO_ALIGN_CENTER,
                                "%d/%d",int(map0.firsthero->mp),int(map0.firsthero->maxmp));

                al_draw_bitmap(expball,180,24,0);
                al_draw_bitmap_region(exper,
                                      0,36-(map0.firsthero->exp/ map0.firsthero->needexp)*36,
                                      36,(map0.firsthero->exp/ map0.firsthero->needexp)*36,
                                      183,27+36-(map0.firsthero->exp/ map0.firsthero->needexp)*36,0);
                al_draw_textf(pixelcyr,al_map_rgb(0,0,0),201,65,ALLEGRO_ALIGN_CENTER,
                                "%d/%d",int(map0.firsthero->exp),int(map0.firsthero->needexp));}


                al_draw_bitmap(commbar,width-300,height-150,0); // INTERACTIVE BAR
                if (selectperson!=NULL)
                {
                    al_draw_textf(calibri,al_map_rgb(255,255,255),width-220,height-140,ALLEGRO_ALIGN_LEFT,
                                "%s",selectperson->name);
                }

                for (int i = 0;i<5;i++) //fast SPELLS bar
                {
                    al_draw_bitmap(spllfrm,0+i*50,height-50,0);
                }

                if (sttngsopen == true)//settings window
                {
                    for (int j = 1; j*20<500;j++)
                    {
                        for (int i = 1;i*20<580;i++)
                        {
                            al_draw_bitmap(wndwbckgrnd,width/2-312+i*20,height/2-262+j*20,0);
                        }
                    }

                    al_draw_bitmap_region(wndwframe,0,0,20,20,width/2-300,height/2-250,0);
                    al_draw_bitmap_region(wndwframe,0,0,20,20,width/2+268,height/2-250,1);
                    al_draw_bitmap_region(wndwframe,0,0,20,20,width/2-300,height/2+218,2);
                    al_draw_bitmap_region(wndwframe,0,0,20,20,width/2+268,height/2+218,3);

                    for (int i=1;i<=28;i++)
                    {
                        al_draw_bitmap_region(wndwframe,20,0,20,20,width/2-300+20*i,height/2-250,0);
                        al_draw_bitmap_region(wndwframe,20,0,20,20,width/2-300+20*i,height/2+218,2);
                    }
                    for (int i=1;i<=23;i++)
                    {
                        al_draw_bitmap_region(wndwframe,40,0,20,20,width/2-300,height/2-250+20*i,0);
                        al_draw_bitmap_region(wndwframe,40,0,20,20,width/2+268,height/2-250+20*i,0);
                    }


                    al_draw_bitmap(setter,width/2-128,height/2+50,0);
                    al_draw_text(calibri,al_map_rgb(0,255,255),width/2,height/2+20,ALLEGRO_ALIGN_CENTRE,"MUSIC VOLUME");
                    al_draw_textf(calibri,al_map_rgb(0,0,0),width/2,height/2+58,ALLEGRO_ALIGN_CENTRE,
                                "%d",volume);

                    al_draw_bitmap(setter,width/2-128,height/2-132,0);
                    al_draw_text(calibri,al_map_rgb(0,255,255),width/2,height/2-152,ALLEGRO_ALIGN_CENTRE,"DISPLAY RESOLUTION");
                    al_draw_textf(calibri,al_map_rgb(0,0,0),width/2,height/2-124,ALLEGRO_ALIGN_CENTRE,
                                "%d*%d",
                                atoi(al_get_config_value(cfg,"display settings","width")),
                                atoi(al_get_config_value(cfg,"display settings","height")));

                    al_draw_bitmap_region(checker,atoi(al_get_config_value(cfg,"display settings","fullscreen"))?32:0,0,
                                          32,32,bttms[FULLSCREEN].x,bttms[FULLSCREEN].y,0);

                    al_draw_text(calibri,al_map_rgb(255,255,0),bttms[FULLSCREEN].x+40,bttms[FULLSCREEN].y+10,ALLEGRO_ALIGN_LEFT,
                                "FullScreen");

                    al_draw_bitmap(bttms[CLOSEOPTIONS].image[0],bttms[CLOSEOPTIONS].x,bttms[CLOSEOPTIONS].y,0);
                }

                if (menuopen == true)//menu window
                {
                    for (int j=0;j<=22;j++)
                        for (int i=0;i<=10;i++)
                            al_draw_bitmap_region(menuback,0,0,16,16,width/2-84+16*i,height/2-184+16*j,0);

                    al_draw_bitmap_region(menuframe,0,0,16,16,width/2-100,height/2-200,0);
                    al_draw_bitmap_region(menuframe,0,0,16,16,width/2+84,height/2-200,1);
                    al_draw_bitmap_region(menuframe,0,0,16,16,width/2-100,height/2+184,2);
                    al_draw_bitmap_region(menuframe,0,0,16,16,width/2+84,height/2+184,3);

                    for (int i=0;i<=10;i++)
                    {
                        al_draw_bitmap_region(menuframe,16,0,16,16,width/2-84+16*i,height/2-200,0);
                        al_draw_bitmap_region(menuframe,16,0,16,16,width/2-84+16*i,height/2+184,2);
                    }
                    for (int i=0;i<=22;i++)
                    {
                        al_draw_bitmap_region(menuframe,32,0,16,16,width/2-100,height/2-184+16*i,0);
                        al_draw_bitmap_region(menuframe,32,0,16,16,width/2+84,height/2-184+16*i,0);
                    }

                    for (int i = BACKTOGAME;i<=QUITTOMAINMENU;i++)
                    {
                        al_draw_bitmap(bttms[i].image[1],bttms[i].x,bttms[i].y,0);
                        al_draw_textf(pixelcyrmenu,al_map_rgb(255,255,0),width/2,bttms[i].y+5,ALLEGRO_ALIGN_CENTER,
                                "%s",bttms[i].name);
                    }
                }
            }
            else if (state==MAINMENU)
            {
                al_clear_to_color(al_map_rgb(50,150,170));

                al_draw_scaled_bitmap(mainmenubackground,0,0,800,600,0,0,width,height,0);
                al_draw_scaled_bitmap(logo,0,0,al_get_bitmap_width(logo),al_get_bitmap_height(logo),0,0,width,width/4,0);

                for (int i = NEWGAME;i<=EXIT;i++)
                {
                    al_draw_scaled_bitmap(bttms[i].image[1],0,(mousestate.x>bttms[i].x and mousestate.x<bttms[i].x+bttms[i].width and mousestate.y>bttms[i].y and mousestate.y<bttms[i].y+bttms[i].height?25:0),100,25,bttms[i].x,bttms[i].y,bttms[i].width,bttms[i].height,0);
                    al_draw_textf(pixelcyrmainmenu,al_map_rgb(220,220,10),bttms[i].x+bttms[i].width/2,bttms[i].y+15,
                                    ALLEGRO_ALIGN_CENTER,"%s",bttms[i].name);
                }
            }

            al_draw_textf(calibri,al_map_rgb(255,0,0),width/2,0,ALLEGRO_ALIGN_CENTER,
                                "FPS=%d",gameFPS);
            for (int i=1;i<5;i++)
            {
                al_draw_textf(calibri,al_map_rgb(255,0,0),width/2,0,ALLEGRO_ALIGN_CENTER,
                                "%s",
                                textline("this is useless text without anything sense, so just read it, scumfuck",calibri,
                                         150,i));
            }

            al_flip_display();
        }
    }

    al_stop_sample_instance(songInstance[0]);

    itoa(volume,sectorname,10);
    al_set_config_value(cfg,"volume settings","music",sectorname);
    if (!al_save_config_file("cfgs/config.cfg",cfg))
        al_show_native_message_box(NULL,NULL,"Settings does not save!","Settings does not save!",NULL,0);

    al_set_system_mouse_cursor(display,ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    al_set_target_bitmap(NULL);

    //USTROY DESTROY
    for (int i=0;i<numblocks;i++)
        al_destroy_bitmap(blocksimage[i]);
    for (int i=0;i<numobj;i++)
        al_destroy_bitmap(objimage[i]);
    for (int i=0;i<nummonsters;i++)
        al_destroy_bitmap(monsterimage[i]);
    for (int i=0;i<numitems;i++)
    {
        al_destroy_bitmap(items[i].image);
        al_destroy_bitmap(items[i].icon);
    }
    al_destroy_bitmap(map0.firsthero->image[0]);
    /*al_destroy_bitmap(map0.firsthero->image[1]);
    al_destroy_bitmap(map0.firsthero->image[2]);
    al_destroy_bitmap(map0.firsthero->image[3]);
    al_destroy_bitmap(map0.firsthero->image[4]);
    al_destroy_bitmap(map0.firsthero->image[5]);*/
    al_destroy_bitmap(imagei);
    al_destroy_bitmap(mainmenubackground);
    al_destroy_bitmap(mainmenubutton);
    al_destroy_bitmap(logo);
    al_destroy_bitmap(sttngsimg);
    al_destroy_bitmap(wndwframe);
    al_destroy_bitmap(wndwbckgrnd);
    al_destroy_bitmap(setter);
    al_destroy_bitmap(checker);
    al_destroy_bitmap(picker);
    al_destroy_bitmap(minimap);
    al_destroy_bitmap(statusbar);
    al_destroy_bitmap(commbar);
    al_destroy_bitmap(tab);
    al_destroy_bitmap(heart);al_destroy_bitmap(health);al_destroy_bitmap(bottle);
    al_destroy_bitmap(mana);al_destroy_bitmap(expball);al_destroy_bitmap(exper);
    al_destroy_bitmap(classicons);
    al_destroy_bitmap(spllfrm);
    al_destroy_bitmap(menubutton);
    al_destroy_bitmap(menuframe);
    al_destroy_bitmap(menuback);
    al_destroy_bitmap(bagimage);al_destroy_bitmap(spellsimage);al_destroy_bitmap(infoimage);
    al_destroy_bitmap(pageimage);
    al_destroy_bitmap(playerframe);
    al_destroy_bitmap(coinimage);
    al_destroy_bitmap(closeimage);
    al_destroy_bitmap(attackindexsword);al_destroy_bitmap(armorindex);
    al_destroy_bitmap(healthbar);al_destroy_bitmap(barhealth);
    al_destroy_bitmap(glorycursor);al_destroy_bitmap(glorycursorarrows);
    al_destroy_bitmap(hellcursor);al_destroy_bitmap(hellcursorarrows);
    al_destroy_bitmap(floorimage[0]);
    al_destroy_bitmap(floorimage[1]);
    al_destroy_bitmap(floorimage[2]);
    al_destroy_bitmap(floorimage[3]);
    al_destroy_event_queue(event_queue);
    al_destroy_config(cfg);
    al_destroy_config(blockscfg);
    al_destroy_config(objcfg);
    al_destroy_config(monsterscfg);
    al_destroy_config(itemscfg);
    al_destroy_config(npccfg);
    al_destroy_font(calibri);
    al_destroy_font(pixelcyr);
    al_destroy_font(pixelcyrmenu);
    al_destroy_font(pixelcyrmainmenu);
    al_destroy_sample(song);
    al_destroy_sample_instance(songInstance[0]);
    al_destroy_sample_instance(songInstance[1]);
    al_destroy_timer(timer);
    al_destroy_display(display);

    return 0;
}

mapp loadmap(block blocks[4],object objects[4],item items[4],monster monsters[4],npc npcs[4])
{
    mapp map0;//var

    return map0;
}

int walkatob(int landmind[MAPMAXSIZEX][MAPMAXSIZEY],int xa,int ya,int xb,int yb)
{
    int now[MAPMAXSIZEX][MAPMAXSIZEY];
    for(int i=0;i<MAPMAXSIZEX;i++)
        for (int j=0;j<MAPMAXSIZEY;j++)
            now[i][j]=landmind[i][j];
    int n=1,a=0;

    now[xb][yb]=n;
    while (now[xa][ya]==0)
    {
        a=0;
        for(int i=0;i<MAPMAXSIZEX;i++)
            for (int j=0;j<MAPMAXSIZEY;j++)
                if (now[i][j]==n)
                {
                    if (now[i+1][j]==0) {now[i+1][j]=n+1;a+=1;}
                    if (now[i][j+1]==0) {now[i][j+1]=n+1;a+=1;}
                    if (now[i-1][j]==0) {now[i-1][j]=n+1;a+=1;}
                    if (now[i][j-1]==0) {now[i][j-1]=n+1;a+=1;}
                }
        if (a==0) return -1;
        n+=1;
    }

    if (now[xa+1][ya]==n-1) return 3;
    if (now[xa][ya+1]==n-1) return 1;
    if (now[xa-1][ya]==n-1) return 2;
    if (now[xa][ya-1]==n-1) return 0;
    return -1;
}
