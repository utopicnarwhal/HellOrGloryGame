#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

class item
{
public:
    const char *name;
    unsigned short int num;
    short int cost;
    short int equiptype;
    short int armor;
    short int damage;
    short int weapontype;

    ALLEGRO_BITMAP *icon;
    ALLEGRO_BITMAP *image;
};

class slot
{
public:
    bool enabled;
    item itemm;
    unsigned short int amount;
};

class person
{
public:
    person *next;
    unsigned short money,dir,x,y;
    float z;
    bool busy,alive;
    short int deal;
    short int damage;
    short int armor,magicresist;
    float sps,aps;
    float hp,maxhp,hpregen,hpregencount;
    float mp,maxmp,mpregen,mpregencount;
    int spellslvl[30];
    bool choosingarea;
    int choosedspell;

    const char *name;
    short int stpcnt,attckcnt,dthcnt;
    short int lvl;

    person(){}
    person(person**& prev);
};

person::person(person**& prev)
{
    *prev=this;
    prev=&next;
}

class hero:public person
{
public:
    float exp,needexp;
    slot equipment[8];
    slot inventory[4][3];
    int attributes[4];
    int attribpoints;
    unsigned short int prof;
    unsigned short num;
    int spellpoints;

    void draw(int height,int width,int zoom,int d);
    int getarmor();
    int getdamage();

    hero():person(){}
    hero(person**& prev):person(prev){}

    ALLEGRO_BITMAP *image[6];
};

void hero::draw(int height,int width,int zoom,int d)
{
    bool leftright=dir==translate(d,LEFT) or dir==translate(d,RIGHT);
    bool downleft=dir==translate(d,DOWN) or dir==translate(d,LEFT);
    if (deal==WALKRIGHT or deal==WALKUP or deal==WALKLEFT or deal==WALKDOWN)
    {
        if ((!downleft) and equipment[WEAPON].enabled) al_draw_scaled_bitmap(image[WEAPONIMAGE],stpcnt>0?32:64,(downleft)?0:48,
                              32,48,width/2-16*zoom,height/2-24*zoom-7,32*zoom,48*zoom,(leftright)?1:0);
        al_draw_scaled_bitmap(image[BODY],stpcnt>0?24:48,(downleft)?0:48,
                              24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[GLOVES].enabled) al_draw_scaled_bitmap(image[ARM],stpcnt>0?24:48,(downleft)?0+96:48,
                              24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[LEGGINGS].enabled) al_draw_scaled_bitmap(image[LEGS],(downleft)?0:24,(stpcnt*sps>0)?48:96,
                              24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[HELM].enabled) al_draw_scaled_bitmap(image[HEAD],(downleft)?0:24,0,
                              24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[ARMOR].enabled) al_draw_scaled_bitmap(image[TRUNK],downleft?0:24,0,
                              24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[GLOVES].enabled) al_draw_scaled_bitmap(image[ARM],stpcnt>0?24:48,(downleft)?0:48+96,
                              24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if ((downleft) and equipment[WEAPON].enabled) al_draw_scaled_bitmap(image[WEAPONIMAGE],stpcnt>0?32:64,(downleft)?0:48,
                              32,48,width/2-16*zoom,height/2-24*zoom-7,32*zoom,48*zoom,(leftright)?1:0);
    }
    else if (deal==NOTHING)
    {
        if ((!downleft) and equipment[WEAPON].enabled) al_draw_scaled_bitmap(image[WEAPONIMAGE],0,(downleft)?0:48,32,48,width/2-16*zoom,height/2-24*zoom-7,32*zoom,48*zoom,(leftright)?1:0);
        al_draw_scaled_bitmap(image[BODY],0,(downleft)?0:48,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[GLOVES].enabled) al_draw_scaled_bitmap(image[ARM],0,(downleft)?0+96:48,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[LEGGINGS].enabled) al_draw_scaled_bitmap(image[LEGS],(downleft)?0:24,0,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[HELM].enabled) al_draw_scaled_bitmap(image[HEAD],(downleft)?0:24,0,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[ARMOR].enabled) al_draw_scaled_bitmap(image[TRUNK],(downleft)?0:24,0,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[GLOVES].enabled) al_draw_scaled_bitmap(image[ARM],0,(downleft)?0:48+96,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if ((downleft) and equipment[WEAPON].enabled) al_draw_scaled_bitmap(image[WEAPONIMAGE],0,(downleft)?0:48,32,48,width/2-16*zoom,height/2-24*zoom-7,32*zoom,48*zoom,(leftright)?1:0);

}
    else if (deal==ATTACK)
    {
        if ((!downleft) and equipment[WEAPON].enabled) al_draw_scaled_bitmap(image[WEAPONIMAGE],96+int(attckcnt*aps)/20*32,(downleft)?0:48,32,48,width/2-16*zoom,height/2-24*zoom-7,32*zoom,48*zoom,(leftright)?1:0);
        al_draw_scaled_bitmap(image[BODY],72+int(attckcnt*aps)/20*24,(downleft)?0:48,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[GLOVES].enabled) al_draw_scaled_bitmap(image[ARM],72+int(attckcnt*aps)/20*24,(downleft)?0+96:48,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[LEGGINGS].enabled) al_draw_scaled_bitmap(image[LEGS],(downleft)?0:24,0,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[HELM].enabled) al_draw_scaled_bitmap(image[HEAD],(downleft)?0:24,0,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[ARMOR].enabled) al_draw_scaled_bitmap(image[TRUNK],(downleft)?0:24,0,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if (equipment[GLOVES].enabled) al_draw_scaled_bitmap(image[ARM],72+int(attckcnt*aps)/20*24,(downleft)?0:48+96,24,48,width/2-12*zoom,height/2-24*zoom-7,24*zoom,48*zoom,(leftright)?1:0);
        if ((downleft) and equipment[WEAPON].enabled) al_draw_scaled_bitmap(image[WEAPONIMAGE],96+int(attckcnt*aps)/20*32,(downleft)?0:48,32,48,width/2-16*zoom,height/2-24*zoom-7,32*zoom,48*zoom,(leftright)?1:0);
    }
}

int hero::getarmor()
{
    armor=(equipment[ARMOR].enabled==true?equipment[ARMOR].itemm.armor:0)+
    (equipment[LEGGINGS].enabled==true?equipment[LEGGINGS].itemm.armor:0)+
    (equipment[HELM].enabled==true?equipment[HELM].itemm.armor:0)+
    (equipment[GLOVES].enabled==true?equipment[GLOVES].itemm.armor:0)+
    (equipment[WEAPON].enabled==true?equipment[WEAPON].itemm.armor:0)+
    (equipment[AMULET].enabled==true?equipment[AMULET].itemm.armor:0)+
    (equipment[ARTIFACT].enabled==true?equipment[ARTIFACT].itemm.armor:0)+
    (equipment[RING].enabled==true?equipment[RING].itemm.armor:0);
    return armor;
}

int hero::getdamage()
{
    damage=(equipment[WEAPON].enabled==true?equipment[WEAPON].itemm.damage:0);
    return damage;
}

class monster: public person
{
public:
    short int height;
    int landmind[MAPMAXSIZEX][MAPMAXSIZEY];
    short int agrrange;
    unsigned short int num;

    monster():person(){}
    monster(person**& prev);

    bool gotoxy(int x,int y);
    void draw(int i,int j,person hero,int width,int height,ALLEGRO_BITMAP *image,ALLEGRO_BITMAP *healthbar,ALLEGRO_BITMAP *barhealth,int zoom,int d);
};

monster::monster(person**& prev):person(prev)
{
    for (int i=0;i<MAPMAXSIZEX;i++)
        for (int j=0;j<MAPMAXSIZEY;j++)
            landmind[i][j]=0;
}

void monster::draw(int i,int j,person hero,int width,int height,ALLEGRO_BITMAP *image,ALLEGRO_BITMAP *healthbar,ALLEGRO_BITMAP *barhealth,int zoom,int d)
{
    bool downleft=dir==translate(d,DOWN) or dir==translate(d,LEFT);
    bool leftup=dir==translate(d,LEFT) or dir==translate(d,UP);
    if (deal==WALKRIGHT or deal==WALKUP or deal==WALKLEFT or deal==WALKDOWN)
    {
        al_draw_scaled_bitmap(image,((stpcnt*sps<15 and stpcnt*sps>0) or stpcnt*sps<-15)?32:64,downleft?0:16*(monster::height+1),32,16*(monster::height+1),
                            (i+j-hero.x-hero.y-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x)-1)*16*zoom+width/2+
                                (M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-M1_POW(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.533,
                            -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+1)*8*zoom+height/2-monster::height*zoom*16+
                                (quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-quasicos(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.266+(hero.z-z)*zoom,
                            32*zoom,16*(monster::height+1)*zoom,leftup?1:0);
    }
    else if (deal==NOTHING)
    {
        al_draw_scaled_bitmap(image,0,downleft?0:16*(monster::height+1),32,16*(monster::height+1),
                            (i+j-hero.x-hero.y-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x)-1)*16*zoom+width/2+M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                            -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+1)*8*zoom+height/2-monster::height*zoom*16+
                                quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.266+(hero.z-z)*zoom,
                            32*zoom,16*(monster::height+1)*zoom,leftup?1:0);
    }
    else if (deal==ATTACK)
    {
        al_draw_scaled_bitmap(image,0,downleft?32*(monster::height+1):32*(monster::height+2),32,16*(monster::height+1),
                            (i+j-hero.x-hero.y-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x)-1)*16*zoom+width/2+
                                M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                            -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+1)*8*zoom+height/2-monster::height*zoom*16+
                                quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.266+(hero.z-z)*zoom,
                            32*zoom,16*(monster::height+1)*zoom,leftup?1:0);
    }
    else if (deal==DEATH)
    {
        al_draw_tinted_scaled_bitmap(image,al_map_rgba(128,128,128,(600-dthcnt)*255/600),32,64,32,16*(monster::height+1),
                            (i+j-hero.x-hero.y-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x)-1)*16*zoom+width/2+
                                (M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-M1_POW(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.533,
                            -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+1)*8*zoom+height/2-monster::height*zoom*16+
                                (quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-quasicos(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.266+(hero.z-z)*zoom,
                            32*zoom,16*(monster::height+1)*zoom,leftup?1:0);
    }
    if (alive==true)
    {
        al_draw_scaled_bitmap(healthbar,0,0,24,7,
                                (i+j-hero.x-hero.y-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x)-0.5)*16*zoom+width/2+
                                (M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-
                                M1_POW(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.533,
                                -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+0.75)*8*zoom+height/2-monster::height*zoom*16+
                                (quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-
                                quasicos(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.266+(hero.z-z)*zoom,
                                24*zoom,7*zoom,0);
        al_draw_scaled_bitmap(barhealth,0,0,(hp/maxhp)*22,5,
                                (i+j-hero.x-hero.y-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x)-0.5)*16*zoom+width/2+zoom+
                                (M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-
                               M1_POW(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.533,
                                -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+0.75)*8*zoom+height/2-monster::height*zoom*16+zoom+
                                (quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps-
                                quasicos(translate((4-d)%4,dir))*stpcnt/2*sps)*zoom*0.266+(hero.z-z)*zoom,
                                22*(hp/maxhp)*zoom,5*zoom,0);
    }
}

class npc:public person
{
public:
    short int num;

    npc():person(){}
    npc(person**& prev):person(prev){}

    ALLEGRO_BITMAP *image;

    void draw(int i,int j,person hero,int width,int height,int zoom,int d);
};

void npc::draw(int i,int j,person hero,int width,int height,int zoom,int d)
{
    al_draw_scaled_bitmap(image,0,0,32,64,
                            (i+j-hero.x-hero.y-1-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x))*16*zoom+width/2+M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                            -8+(i+j-hero.x-hero.y-(d%3!=0)*2*(j-hero.y)-(d<2)*2*(i-hero.x)+1)*8*zoom+height/2-3*zoom*16+
                                quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.266+(hero.z-z)*zoom,
                            32*zoom,64*zoom,0);
}

class object
{
public:
    object *next;
    const char *name;
    bool firm;
    int x,y,z;
    bool animation;
    int nu,counter;
    int num;
    int dir;
    int height;
    object(){}
    object(object**&prev);
};

object::object(object**& prev)
{
    *prev=this;
    prev=&next;
}

class block
{
public:
    int num;
    short int height;
    short int enterz;
    int dir;
    bool enters[4];
    bool stair;
    int up;
    bool firm;
    const char *name;
    unsigned short int width,length;
    short int floor;
    ALLEGRO_COLOR color;
};

class mapp
{
public:
    int sizex;
    int sizey;
    int numobj,nummonsters,numnpc,numplayers;
    block coord[MAPMAXSIZEX][MAPMAXSIZEY];
    object *firstobj;
    object **pnewobj;
    hero *firsthero;
    person **pnewhero;
    monster *firstmonster;//указатель на связанный список монстров
    person **pnewmonster;
    npc *firstnpc;
    person **pnewnpc;
    mapp();
    bool checkway(person person1,person person2);
    bool checkway(person person1,int dir);
    int numimg(int i,int j);
    void focusing(short int *focusx,short int *focusy,person hero,int width,int height,int zoom,ALLEGRO_MOUSE_STATE mousestate,int d);
    void drawfloor(int i,int j,ALLEGRO_BITMAP *blocksimage[4],ALLEGRO_BITMAP *objimage[4],int imagenum);
    void drawblock(int i,int j,person hero,int width,int height,ALLEGRO_BITMAP *blocksimage[4],ALLEGRO_BITMAP *objimage[4],int zoom,int focusx,int focusy,int d);
};

mapp::mapp()
{
    pnewobj=&firstobj;
    pnewhero=(person**)&firsthero;
    pnewmonster=(person**)&firstmonster;
    pnewnpc=(person**)&firstnpc;
}

bool mapp::checkway(person person1,int dir)
{
    switch (dir)
    {
    case 0:
        if (coord[person1.x][person1.y-1].firm==false or ((
            (coord[person1.x][person1.y-1].up+coord[person1.x][person1.y-1].enterz==
                coord[person1.x][person1.y].up+coord[person1.x][person1.y].enterz)
            or coord[person1.x][person1.y-1].enterz==coord[person1.x][person1.y].enterz)
            and coord[person1.x][person1.y-1].enters[translate(coord[person1.x][person1.y-1].dir,0)]
            and coord[person1.x][person1.y].enters[translate(coord[person1.x][person1.y].dir,1)])
            or (coord[person1.x][person1.y-1].dir==coord[person1.x][person1.y].dir
                and coord[person1.x][person1.y-1].num==coord[person1.x][person1.y].num))
            {for (object* iobj=firstobj;iobj!=NULL;iobj=iobj->next)
                if (iobj->x==person1.x and iobj->y==person1.y-1 and iobj->firm==true) return false;
            return true;}
        break;
    case 1:
        if (coord[person1.x][person1.y+1].firm==false or ((
            (coord[person1.x][person1.y+1].up+coord[person1.x][person1.y+1].enterz==
                coord[person1.x][person1.y].up+coord[person1.x][person1.y].enterz)
            or coord[person1.x][person1.y+1].enterz==coord[person1.x][person1.y].enterz)
            and coord[person1.x][person1.y+1].enters[translate(coord[person1.x][person1.y+1].dir,1)]
            and coord[person1.x][person1.y].enters[translate(coord[person1.x][person1.y].dir,0)])
            or (coord[person1.x][person1.y+1].dir==coord[person1.x][person1.y].dir
                and coord[person1.x][person1.y+1].num==coord[person1.x][person1.y].num))
            {for (object* iobj=firstobj;iobj!=NULL;iobj=iobj->next)
                if (iobj->x==person1.x and iobj->y==person1.y+1 and iobj->firm==true) return false;
            return true;}
        break;
    case 2:
        if (coord[person1.x-1][person1.y].firm==false or ((
            (coord[person1.x-1][person1.y].up+coord[person1.x-1][person1.y].enterz==
                coord[person1.x][person1.y].up+coord[person1.x][person1.y].enterz)
            or coord[person1.x-1][person1.y].enterz==coord[person1.x][person1.y].enterz)
            and coord[person1.x-1][person1.y].enters[translate(coord[person1.x-1][person1.y].dir,2)]
            and coord[person1.x][person1.y].enters[translate(coord[person1.x][person1.y].dir,3)])
            or (coord[person1.x-1][person1.y].dir==coord[person1.x][person1.y].dir
                and coord[person1.x-1][person1.y].num==coord[person1.x][person1.y].num))
            {for (object* iobj=firstobj;iobj!=NULL;iobj=iobj->next)
                if (iobj->x==person1.x-1 and iobj->y==person1.y and iobj->firm==true) return false;
            return true;}
        break;
    case 3:
        if (coord[person1.x+1][person1.y].firm==false or ((
            (coord[person1.x+1][person1.y].up+coord[person1.x+1][person1.y].enterz==
                coord[person1.x][person1.y].up+coord[person1.x][person1.y].enterz)
            or coord[person1.x+1][person1.y].enterz==coord[person1.x][person1.y].enterz)
            and coord[person1.x+1][person1.y].enters[translate(coord[person1.x+1][person1.y].dir,3)]
            and coord[person1.x][person1.y].enters[translate(coord[person1.x][person1.y].dir,2)])
            or (coord[person1.x+1][person1.y].dir==coord[person1.x][person1.y].dir
                and coord[person1.x+1][person1.y].num==coord[person1.x][person1.y].num))
            {for (object* iobj=firstobj;iobj!=NULL;iobj=iobj->next)
                if (iobj->x==person1.x+1 and iobj->y==person1.y and iobj->firm==true) return false;
            return true;}
        break;
    }
    return false;
}

bool mapp::checkway(person person1,person person2)
{
    switch (person1.dir)
    {
    case 0:
        if (((person2.x==person1.x and person2.y==person1.y-1) or
            (((person2.deal==WALKDOWN and person2.x==person1.x and person2.y==person1.y-2) or
            (person2.deal==WALKRIGHT and person2.x==person1.x-1 and person2.y==person1.y-1) or
            (person2.deal==WALKLEFT and person2.x==person1.x+1 and person2.y==person1.y-1)) and person2.stpcnt>0))
            and person2.alive==true)
            return false;
        else if (person2.next!=NULL) return checkway(person1,*person2.next); else return true;
        break;
    case 1:
        if (((person2.x==person1.x and person2.y==person1.y+1) or
            (((person2.deal==WALKUP and person2.x==person1.x and person2.y==person1.y+2) or
            (person2.deal==WALKRIGHT and person2.x==person1.x-1 and person2.y==person1.y+1) or
            (person2.deal==WALKLEFT and person2.x==person1.x+1 and person2.y==person1.y+1)) and person2.stpcnt>0))
            and person2.alive==true)
            return false;
        else if (person2.next!=NULL) return checkway(person1,*person2.next); else return true;
        break;
    case 2:
        if (((person2.x==person1.x-1 and person2.y==person1.y) or
            (((person2.deal==WALKUP and person2.x==person1.x-1 and person2.y==person1.y+1) or
            (person2.deal==WALKRIGHT and person2.x==person1.x-2 and person2.y==person1.y) or
            (person2.deal==WALKDOWN and person2.x==person1.x-1 and person2.y==person1.y-1)) and person2.stpcnt>0))
            and person2.alive==true)
            return false;
        else if (person2.next!=NULL) return checkway(person1,*person2.next); else return true;
        break;
    case 3:
        if (((person2.x==person1.x+1 and person2.y==person1.y) or
            (((person2.deal==WALKUP and person2.x==person1.x+1 and person2.y==person1.y+1) or
            (person2.deal==WALKLEFT and person2.x==person1.x+2 and person2.y==person1.y) or
            (person2.deal==WALKDOWN and person2.x==person1.x+1 and person2.y==person1.y-1)) and person2.stpcnt>0))
            and person2.alive==true)
            return false;
        else if (person2.next!=NULL) return checkway(person1,*person2.next); else return true;
    }
    return true;
}

int mapp::numimg(int i,int j)
{
    int a = 0,b = 0;
    while (coord[i-(b+1)*(-minisin(coord[i][j].dir))][j-(b+1)*minicos(coord[i][j].dir)].num==coord[i][j].num and b<coord[i][j].length-1)
        b+=1;
    while (coord[i-(a+1)*minicos(coord[i][j].dir)][j-(a+1)*(-minisin(coord[i][j].dir))].num==coord[i][j].num and a<coord[i][j].width-1)
        a+=1;
    return (b*coord[i][j].width)+a;
}

void mapp::drawfloor(int i,int j,ALLEGRO_BITMAP *blocksimage[4],ALLEGRO_BITMAP *objimage[4],int d)
{
    if (coord[i][j].floor!=-1)
        al_draw_scaled_bitmap(blocksimage[coord[i][j].floor],
                        0,0+(3-(d+coord[i][j].dir)%4)*16,32,16,
                        (i+j-(d>=2)*(2*j-sizey+1)-(d%3!=0)*(2*i-sizex+1))*16,
                        (-8+(i+j+1-(d<=1)*(2*i-sizex+1)-(d%3!=0)*(2*j-sizey+1))*8),
                        32,16,0);
    else al_draw_scaled_bitmap(blocksimage[coord[i][j].num],
                        0+numimg(i,j)*32,0+(3-(d+coord[i][j].dir)%4)*16,32,16+16*coord[i][j].height,
                        (i+j-(d>=2)*(2*j-sizey+1)-(d%3!=0)*(2*i-sizex+1))*16,
                        (-8+(i+j+1-(d<=1)*(2*i-sizex+1)-(d%3!=0)*(2*j-sizey+1))*8-coord[i][j].height*16),
                        32,(coord[i][j].height+1)*16,0);

    for (object* iobj = firstobj;iobj!=NULL;iobj=iobj->next)
    {
        if (iobj->x==i and iobj->y==j and iobj->height<1 and iobj->animation==0)
            al_draw_scaled_bitmap(objimage[iobj->num],
                    0,0+(3-(d+coord[i][j].dir)%4)*16,32,16+16*iobj->height,
                    (i+j-(d>=2)*(2*j-sizey+1)-(d%3!=0)*(2*i-sizex+1))*16,
                    -8+(i+j+1-(d<=1)*(2*i-sizex+1)-(d%3!=0)*(2*j-sizey+1))*8-iobj->height*16,
                    32,(iobj->height+1)*16,0);
    }
}

void mapp::drawblock(int i,int j,person hero,int width,int height,ALLEGRO_BITMAP *blocksimage[4],ALLEGRO_BITMAP *objimage[4],int zoom,int focusx,int focusy,int d)
{
    if (((((hero.x-i)*quasicos(d-1)==(j-hero.y)*quasicos(d) or (hero.x-i)*quasicos(d-1)-1==(j-hero.y)*quasicos(d) or (hero.x-i)*quasicos(d-1)==(j-hero.y)*quasicos(d)-1)
        and (hero.x-i)*quasicos(d-1)>=0 and (j-hero.y)*quasicos(d)>=0
        and (coord[i][j].height-(coord[hero.x][hero.y].enterz+coord[hero.x][hero.y].up)>=(hero.x-i)*quasicos(d-1)
        or coord[i][j].height-(coord[hero.x][hero.y].enterz+coord[hero.x][hero.y].up)>=(j-hero.y)*quasicos(d)))
        or (((focusx-i)*quasicos(d-1)==(j-focusy)*quasicos(d) or (focusx-i)*quasicos(d-1)-1==(j-focusy)*quasicos(d) or (focusx-i)*quasicos(d-1)==(j-focusy)*quasicos(d)-1)
        and (focusx-i)*quasicos(d-1)>=0 and (j-focusy)*quasicos(d)>=0 and coord[focusx][focusy].height<coord[i][j].height
        and (coord[i][j].height-coord[focusx][focusy].height>=(focusx-i)*quasicos(d-1)
        or coord[i][j].height-coord[focusx][focusy].height>=(j-focusy)*quasicos(d))))and coord[i][j].floor!=-1)
        al_draw_tinted_scaled_bitmap(blocksimage[coord[i][j].num],al_map_rgba(100,100,100,100),
                        0+numimg(i,j)*32,16*(coord[i][j].height+1)*((d+coord[i][j].dir)%4),32,16+16*coord[i][j].height,
                        (i+j-hero.x-hero.y-1-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x))*16*zoom+width/2+M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                        -8+(i+j-hero.x-hero.y+1-(d<=1)*2*(i-hero.x)-(d%3!=0)*2*(j-hero.y))*8*zoom+height/2-coord[i][j].height*16*zoom+quasicos(translate((4-d)%4,hero.dir))*
                                hero.stpcnt/2*hero.sps*zoom*0.266+hero.z*zoom,
                        32*zoom,(coord[i][j].height+1)*16*zoom,0);
    else if (coord[i][j].floor!=-1)
            al_draw_scaled_bitmap(blocksimage[coord[i][j].num],
                        0+numimg(i,j)*32,16*(coord[i][j].height+1)*((d+coord[i][j].dir)%4),32,16+16*coord[i][j].height,
                        (i+j-hero.x-hero.y-1-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x))*16*zoom+width/2+M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                        -8+(i+j-hero.x-hero.y+1-(d<=1)*2*(i-hero.x)-(d%3!=0)*2*(j-hero.y))*8*zoom+height/2-coord[i][j].height*16*zoom+quasicos(translate((4-d)%4,hero.dir))*
                                hero.stpcnt/2*hero.sps*zoom*0.266+hero.z*zoom,
                        32*zoom,(coord[i][j].height+1)*16*zoom,0);
    for (object *iobj = firstobj;iobj!=NULL;iobj=iobj->next)
        if (iobj->x==i and iobj->y==j and (iobj->height>0 or iobj->animation>=1) and
            ((((hero.x-i)*quasicos(d-1)==(j-hero.y)*quasicos(d) or (hero.x-i)*quasicos(d-1)-1==(j-hero.y)*quasicos(d) or (hero.x-i)*quasicos(d-1)==(j-hero.y)*quasicos(d)-1)
            and (hero.x-i)*quasicos(d-1)>=0 and (j-hero.y)*quasicos(d)>=0 and coord[hero.x][hero.y].height<iobj->height
            and (iobj->height>=(hero.x-i)*quasicos(d-1) or iobj->height>=(j-hero.y)*quasicos(d)))
            or (((focusx-i)*quasicos(d-1)==(j-focusy)*quasicos(d) or (focusx-i)*quasicos(d-1)-1==(j-focusy)*quasicos(d) or (focusx-i)*quasicos(d-1)==(j-focusy)*quasicos(d)-1)
            and (focusx-i)*quasicos(d-1)>=0 and (j-focusy)*quasicos(d)>=0 and coord[focusx][focusy].height<iobj->height
            and (iobj->height-coord[focusx][focusy].height>=(focusx-i)*quasicos(d-1)
            or iobj->height-coord[focusx][focusy].height>=(j-focusy)*quasicos(d)))))
            al_draw_tinted_scaled_bitmap(objimage[iobj->num],al_map_rgba(100,100,100,100),
                        (iobj->counter*iobj->nu<60)?(0):(32),16*(iobj->height+1)*((d+coord[i][j].dir)%4),32,16+16*iobj->height,
                        (i+j-hero.x-hero.y-1-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x))*16*zoom+width/2+M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                        -8+(i+j-hero.x-hero.y+1-(d<=1)*2*(i-hero.x)-(d%3!=0)*2*(j-hero.y))*8*zoom+height/2-(iobj->height+coord[i][j].height)*16*zoom+quasicos(translate((4-d)%4,hero.dir))*
                                hero.stpcnt/2*hero.sps*zoom*0.266+hero.z*zoom,
                        32*zoom,(iobj->height+1)*16*zoom,0);
        else if (iobj->x==i and iobj->y==j and (iobj->height>0 or iobj->animation>=1)) al_draw_scaled_bitmap(objimage[iobj->num],
                        (iobj->counter*iobj->nu<60)?(0):(32),16*(iobj->height+1)*((d+coord[i][j].dir)%4),32,16+16*iobj->height,
                        (i+j-hero.x-hero.y-1-(d>=2)*2*(j-hero.y)-(d%3!=0)*2*(i-hero.x))*16*zoom+width/2+M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
                        -8+(i+j-hero.x-hero.y+1-(d<=1)*2*(i-hero.x)-(d%3!=0)*2*(j-hero.y))*8*zoom+height/2-(iobj->height+coord[i][j].height)*16*zoom+quasicos(translate((4-d)%4,hero.dir))*
                                hero.stpcnt/2*hero.sps*zoom*0.266+hero.z*zoom,
                        32*zoom,(iobj->height+1)*16*zoom,0);
}

struct buttom
{
    const char *name;
    bool visible;
    int x,y,width,height;
    ALLEGRO_BITMAP *image[2];
};

void mapp::focusing(short int *focusx,short int *focusy,person hero,int width,int height,int zoom,ALLEGRO_MOUSE_STATE mousestate,int d)
{
    int x=mousestate.x-((*focusx-hero.x)*(quasicos(d))+(*focusy-hero.y)*(quasicos(d-1)))*16*zoom-width/2-M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533,
        y=mousestate.y+8-((*focusx-hero.x)*(quasicos(d+1))+(*focusy-hero.y)*(quasicos(d))+2)*8*zoom-height/2-quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.266;

    while ((((y>-(x)/2+8*zoom+(hero.z-coord[*focusx][*focusy].enterz)*zoom and x>0) or (x>16*zoom and y<0)) or
           ((y<-(x)/2-8*zoom+(hero.z-naib(coord[*focusx+minicos(d-1)][*focusy-minicos(d)].enterz,coord[*focusx][*focusy].enterz))*zoom and x<0)
                          or (x<-16*zoom and y>0)) or
           ((y>(x)/2+8*zoom+(hero.z-coord[*focusx][*focusy].enterz)*zoom and x<0) or (x<-16*zoom and y<0)) or
           ((y<(x)/2-8*zoom+(hero.z-naib(coord[*focusx+minicos(d)][*focusy+minicos(d-1)].enterz,coord[*focusx][*focusy].enterz))*zoom and x>0)
                          or (x>16*zoom and y>0)))==1)
            {
                if ((y>-(x)/2+8*zoom+(hero.z-coord[*focusx][*focusy].enterz)*zoom and x>0) or (x>16*zoom and y<0))
                    {(d%2==0)?(*focusy-=quasicos(d+1)):(*focusx+=quasicos(d));}
                else if ((y<-(x)/2-8*zoom+(hero.z-naib(coord[*focusx+minicos(d-1)][*focusy-minicos(d)].enterz,coord[*focusx][*focusy].enterz))*zoom and x<0)
                          or (x<-16*zoom and y>0))
                    {(d%2==0)?(*focusy+=quasicos(d+1)):(*focusx-=quasicos(d));}
                else if ((y>(x)/2+8*zoom+(hero.z-coord[*focusx][*focusy].enterz)*zoom and x<0) or (x<-16*zoom and y<0))
                    {(d%2==0)?(*focusx-=quasicos(d-1)):(*focusy+=quasicos(d+1));}
                else if ((y<(x)/2-8*zoom+(hero.z-naib(coord[*focusx+minicos(d)][*focusy+minicos(d-1)].enterz,coord[*focusx][*focusy].enterz))*zoom and x>0)
                          or (x>16*zoom and y>0))
                    {(d%2==0)?(*focusx+=quasicos(d-1)):(*focusy-=quasicos(d+1));}

                if (*focusx<1) {*focusx=1;break;}
                else if (*focusx>sizex-2) {*focusx=sizex-2;break;}
                if (*focusy<1) {*focusy=1;break;}
                else if (*focusy>sizey-2) {*focusy=sizey-2;break;}

                x=mousestate.x-((*focusx-hero.x)*(quasicos(d))+(*focusy-hero.y)*(quasicos(d-1)))*16*zoom-width/2-M1_POW(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.533;
                y=mousestate.y+8-((*focusx-hero.x)*(quasicos(d+1))+(*focusy-hero.y)*(quasicos(d))+2)*8*zoom-height/2-quasicos(translate((4-d)%4,hero.dir))*hero.stpcnt/2*hero.sps*zoom*0.266;
            }
        if (*focusx<1) *focusx=1;
        else if (*focusx>sizex-2) *focusx=sizex-2;
        if (*focusy<1) *focusy=1;
        else if (*focusy>sizey-2) *focusy=sizey-2;
}

mapp map0;

class spell
{
public:
    const char *name,*doing,*description;
    bool passive;
    int num,doingnum,manacost,lineskill,row;
    ALLEGRO_BITMAP *icon;

    bool doingfunc(person *caster,int focusx,int focusy,object objects[5]);
};

bool spell::doingfunc(person *caster,int focusx,int focusy,object objects[5])
{
    int counti=0;
    char trnslte[50];
    for (int d=0;(d<doingnum);d++)
    {
        for (int i=0;i<50;i++)
            trnslte[i]=0;

        for (int i=0;(*(doing+i+counti)!=*(" ")) and ((*(doing+i+counti)!=*("(")));i++)
        {
            trnslte[i]=*(doing+i+counti);
        }

        if (strcmp(trnslte,"teleport")==0 and caster->mp>=manacost and caster->alive==true)
        {
            caster->x=focusx;caster->y=focusy;caster->stpcnt=0;caster->deal=NOTHING;caster->busy=false;
            caster->mp-=manacost;
        }else
        if (strcmp(trnslte,"maxhpplus")==0)
        {
            caster->maxhp+=atoi(doing+counti+10)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"armorplus")==0)
        {
            caster->armor+=atoi(doing+counti+10)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"maxmpplus")==0)
        {
            caster->maxmp+=atoi(doing+counti+10)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"damageplus")==0)
        {
            caster->damage+=atoi(doing+counti+11)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"spsplus")==0)
        {
            caster->sps+=atof(doing+counti+9)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"apsplus")==0)
        {
            caster->aps+=atof(doing+counti+9)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"hpregenplus")==0)
        {
            caster->hpregen+=atof(doing+counti+13)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"mpregenplus")==0)
        {
            caster->mpregen+=atof(doing+counti+13)*caster->spellslvl[num];
        }else
        if (strcmp(trnslte,"magicresistplus")==0)
        {
            caster->magicresist+=atoi(doing+counti+17)*caster->spellslvl[num];
        }
        if (strcmp(trnslte,"createobj")==0 and caster->mp>=manacost and caster->alive==true)
        {
            if (caster->choosingarea==0)
            {
                caster->choosingarea=1;
                caster->choosedspell=num;
            }
            else
            {
                caster->mp-=manacost;
                caster->choosingarea=0;
                caster->choosedspell=-1;
                object* obj=new object(map0.pnewobj);
                map0.numobj+=1;
                obj->num=atoi(doing+counti+10);
                obj->x=focusx;
                obj->y=focusy;
                obj->dir=0;
                obj->name=objects[obj->num].name;
                obj->firm=objects[obj->num].firm;
                obj->height=objects[obj->num].height;
                obj->animation=objects[obj->num].animation;
                if (obj->animation>=1) obj->nu=objects[obj->num].nu;
                else obj->nu=0;
                obj->counter=0;
                obj->z=0;
                obj->next=NULL;
            }
        }
        for (int j=counti;(j<50) and ((*(doing+counti)!=*(";"))!=0);j++) counti=j;
        counti+=1;
    }
    return 1;
}

const char *textline(const char *s,ALLEGRO_FONT *font,int pxwidth,int linenum)
{
    const char *s1;
    int d=0,c=0;
    for (int i=1;i<linenum;i++) // переходит на указанную строку
    {
        while (pxwidth>al_get_text_width(font,s1))
        {
            d+=1;
            *(s1+d)=*(s+d+c);
        }
        c=c+d-1;
        for (int j=0;j<=d;j++)
            *(s1+j)=0;
        d=0;
    }
    while (pxwidth>al_get_text_width(font,s1))
    {
        d+=1;
        *(s1+d)=*(s+d+c);
    }
    *(s1+d)=0;
    return s1;
}

int charlength(int c){
  int a=1;
  while(c&128){
    c<<=1;
 a++;}
  return a;}

char* textline(char*s0,int width,FONT f){
  char c;
  char *s=s0;
  int length,cl=0;
  do{
    s+=cl;
    c=*s;
    if((c=='\000')||(c=='\n'))break;
    *s='\000';
    length=getstringlength(f,s0);
    *s=c;
    cl=charlength(c);
  }while(length<=width);
  return s;}

#endif // HEADER_H_INCLUDED
