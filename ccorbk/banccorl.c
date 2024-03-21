#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */
#include <string.h>
#include <math.h>
#define MAXTRAN 10000
#define MAXMON 100
#define MAXCONT 100
#define MAXRATA 200
float rata[MAXRATA];
float val[MAXRATA][MAXRATA]; //ratele instrumentelor
char numeban[MAXRATA][7]; //sirurile de caractere care definesc numele instrumentelor
int x,y,nrban,nrrt;
int an[MAXTRAN]; //anul in care s-a facut tranzactia
char luna[MAXTRAN][4]; //luna in care s-a facut tranzactia
int zi[MAXTRAN]; //ziua in care s-a facut tranzactia
char moneda[MAXTRAN][7]; //moneda in care s-a facut tranzactia
int suma[MAXTRAN]; //suma cu care s-a facut tranzactia
int ora[MAXTRAN]; //ora la care s-a facut tranzactia
int min[MAXTRAN]; //minutul la care s-a facut tranzactia
char dela[MAXTRAN][8]; //contul din care s-au scos banii in tranzactie
char del[MAXCONT][8]; //conturile din care s-au scos bani
char catr[MAXCONT][8]; //conturile in care s-au transferat bani
char catre[MAXTRAN][8]; //contul in care s-au transferat bani in tranzactie
char repr[MAXTRAN][45]; //ce reprezinta tranzactia
char mon_rez[MAXMON][7];
char mon_ter[MAXMON][7];
float rata[MAXRATA];
char mon[MAXMON][7]; //monedele cu care s-au facut tranzactii
int sum[MAXMON]; //suma totala a unui instrument
int dob[MAXCONT];
int nrec; //numar tranzactii
int nmon=0; //numar instrumente
int c, nde=0,nca=0;
int gasita;

int read_rate() //citeste fisierul rate.dat si creeaza matricea de rate
{
    int i,j;
    char buffer[256];
    char *s;
    int k,l;
    char mond[7],monc[7];
    FILE * rat;
    char fnam[100]="rate.dat";
    nrrt = 0;
    rat = fopen("rate.dat", "r");
    if (rat==NULL) {
      fprintf(stderr,"can't open file %s",fnam);
	exit(-1);
     }
    for (i=0;i<MAXRATA;++i) for (j=0;j<MAXRATA;++j)
    {
        if (i==j) val[i][j]=1.0;
        else val[i][j]=0.0;
    }
    nrban=0;
    while(1)
    {
        s=fgets (buffer, 256, rat);
        if (s==NULL) break;
        for (i=0;i<=5;++i) mond[i]=buffer[i];
        mond[i] = '\0';
        for (i=7;i<=12;++i) monc[i-7]=buffer[i];
        monc[i-7] = '\0';
        k=l=1;
        for (i=0;i<=nrban;++i)
        {
            if (strcmp(mond,numeban[i])==0 && k==1)
            {
                x=i;
                k=0;
            }
            if (strcmp(monc,numeban[i])==0 && l==1)
            {
                y=i;
                l=0;
            }
            if (k==0 && l==0) break;
        }
        if (k==1)
        {
            strcpy(numeban[nrban],mond);
            x=nrban;
            ++nrban;
        }
        if (l==1)
        {
            strcpy(numeban[nrban],monc);
            y=nrban;
            ++nrban;
        }
        val[x][y] = atof(buffer+14);
        val[y][x] = 1/atof(buffer+14);
    }
    fclose(rat);
    nrrt=j*2-1;
}
int complete_rate() // completeaza matricea de rate
{
    int k;
    int i,j;
    do{
        k=0;
        for (x=0;x<=nrban;++x)
	  for (y=0;y<=nrban;++y) {
	      if (val[x][y]==0.0) {
		for (i=0;i<=nrban;++i) {
		    if (val[i][x] !=0 && val[i][y] !=0) {
			val[x][y]=val[x][i]*val[i][y];
			val[y][x]=1/val[x][y];
			k=1;
			break;
		    }
		}
	      }
        }
    }while (k==1);
}
int read_tranz () //citeste fisierul tranz.dat si creeaza vectorii corespunzatori
{
  int i,j;
  char buffer[256];
  char *s;
  FILE * tr;
  tr = fopen("tranz7997.dat", "r");
  //fgets (buffer, 256, tr);
  //printf ("Prima linie este: %s\n",buffer);
  fgets (buffer, 256, tr);
  //i = atoi (buffer);
  //printf ("A doua linie este: %s\n",buffer);
  for (j=0;;++j)
  {
    s=fgets (buffer,256,tr);
    if (s==NULL)
    {
	nrec=j;
	break;
    }
    zi[j] = atoi(buffer);
    for (i = 5;i<=7;++i) luna[j][i-5]=buffer[i];
    luna[j][i-5] = '\0';
    an[j] = atoi(buffer+10);
    ora[j] = atoi(buffer+15);
    min[j] = atoi(buffer+19);
    suma[j] = atoi(buffer+22)*100+atoi(buffer+27);
    for (i = 30;i<=35;++i) moneda[j][i-30]=buffer[i];
    moneda[j][i-30] = '\0';
    for (i = 38;i<=44;++i) dela[j][i-38]=buffer[i];
    dela[j][i-38] = '\0';
    //printf("%s \n",dela[j]);
    for (i = 50;i<=56;++i) catre[j][i-50]=buffer[i];
    catre[j][i-50] = '\0';
    for (i = 60;i<=120;++i) repr[j][i-60]=buffer[i];
  }
  fclose(tr);
}
void usage() //tipareste usage
    {printf("Usage: \n banccorl cont [nume cont]  --- Afiseaza continutul contului \n banccorl instrument [moneda]  --- Afiseaza descrierea instrumentului si suma care se afla in banca \n banccorl tabel  --- Afiseaza tabelul cu toate instrumentele \n banccorl conv [moneda] [moneda] [suma]  --- Converteste suma din prima moneda in a doua \n");}
int ext_mon() //adauga instrumentele din tranz.dat intr-un vector de stringuri
{
  int j,i;
  nmon=0;
  for (i=0;i<=nrec;++i)
  {
    gasita=0;
    for (j=0;j<=nmon;++j)
    {
      if (!strcmp(moneda[i],mon[j]))
      {
        gasita=1;
        break;
      }
    }
    if (!gasita)
    {
      for (j=0;j<7;++j) mon[nmon][j]=moneda[i][j];
      ++nmon;
    }
  }
}

int ext_cont() //adauga conturile din tranz.dat intr-un vector de stringuri
{
  int i,j;
  nde=0;
  nca=0;
  for (i=0;i<nrec;++i)
  {
    gasita=0;
    for (j=0;j<nde;++j)
    {
      if (!strcmp(dela[i],del[j]))
      {
        gasita=1;
        break;
      }
    }
    if (!gasita)
    {
      for (j=0;j<8;++j) del[nde][j]=dela[i][j];
      ++nde;
    }
  }
  for (i=0;i<=nrec;++i)
  {
    gasita=0;
    for (j=0;j<nde;++j)
    {
      if (!strcmp(catre[i],del[j]))
      {
        gasita=1;
        break;
      }
    }
    if (!gasita)
    {
      for (j=0;j<8;++j) del[nde][j]=catre[i][j];
      ++nde;
    }
  }
}

/*int read_rate()
{
  int i,j;
  char *s;
 rat = fopen("rate.dat", "r");
  for (j=0;;++j)
  {
    s=fgets (buffer,256,tr);
    if (s==NULL)
    {
	break;
    }
  }

}
*/
void trnz()
{
  int i;
  for (i=1;i<nrec;++i) printf("%d|%s|%d|%d|%d|%d.%d|%s|%s|%s|%s\n", zi[i],luna[i],an[i],ora[i],min[i],suma[i]/100,suma[i]%100,moneda[i],dela[i],catre[i],repr[i]);
}
float convv(char argm1[7], char argm2[7], float amnt) //converteste din instrumentul 1 (argm1) in instrumentul 2 (argm2) valoarea amnt
{
  float b;
  int l,k;
  y=0;
  k=0;
  l=0;
  for (x=0;x<=nrban;++x) {
    if (strcmp(numeban[x],argm1) == 0)
    {
      k=1;
      break;
    }
    if (strcmp(numeban[x],argm2) == 0)
    {
      l=1;
      y = x;
    }
  }
  if (l==0) for (y=x;y<=nrban;++y) if (strcmp(numeban[y],argm2) == 0) {
    l=1;
    break;
  }
  b = amnt;
  //printf("%f \n",b*val[x][y]);
  if (k==1 && l==1 && b>=0) 
    {
      return b*val[y][x];
      //printf ("%8.2f %s = %8.2f %s\n", b, numeban[x] , val[y][x]*b, numeban[y]);
    }
  else return b;
}
int main(int argc, char *argv[])
{
  char nam[8];
  int i,j,k=0;
  read_tranz();
  ext_mon();
  system("./makerate");
  read_rate(); //***
  ext_cont();
  //printf("%d", argc);
  complete_rate();
  //printf("%s \n", dela[1]);
//for (i=0;i<nrec;++i) printf("%s     %s      \n",dela[i],catre[i]);
  for (i=0;i<nmon;++i) sum[i]=0;
  for (i=0;i<nde;++i)  dob[i]=0;
  if (argc==1)
  {
  usage();
  return 0;
  }
  else if (strcmp(argv[1],"tranz") == 0) trnz();
  else if (strcmp(argv[1],"cont") == 0)
  {
    int summ=0;
    strcpy(nam,argv[2]);
    printf("Extras al contului %s \n",nam);
    k=0;
    for (i=0;i<7;++i)
    {
      if (nam[i]==0) k=1;
      if (k==1) nam[i]=' ';
    }
    nam[7] = 0;
    for (i=0;i<nmon;++i)
    {
      printf ("%s: ",mon[i]);
      int cm=0;
      for (j=0;j<nrec;++j)
      {
        if ((strcmp(catre[j],nam) == 0) && (strcmp(mon[i],moneda[j]) == 0))
        {
          cm+=suma[j];
        }
        if ((strcmp(dela[j],nam) == 0) && (strcmp(mon[i],moneda[j]) == 0))
        {
          cm-=suma[j];
        }
      }
      //summ+=convv(mon[i],"   RON",cm);
      int ronsm;
      if (cm>0) ronsm=convv(mon[i],"   RON",cm);
      if (cm<0) ronsm=-convv(mon[i],"   RON",abs(cm));
      if (cm==0) ronsm=0;
      if (cm>0) summ+=convv(mon[i],"   RON",cm);
      if (cm<0) summ-=convv(mon[i],"   RON",abs(cm));
      //if (strcmp(mon[i],"   VIC")==0) printf("\n %s     %d    %f \n",mon[i],cm,convv(mon[i],"   RON",cm));
      //qwqprintf("suma: %d \n",summ);
      printf ("%10d.%02d  --->  %10d.%02d RON\n",cm/100,abs(cm%100),ronsm/100,abs(ronsm%100));
      //printf ("%d.%d  --->  %d.%d RON\n",summ/100,abs(summ%100),ronsm/100,abs(ronsm%100));
    }
    printf ("Total RON: %28d.%02d RON\n",summ/100,abs(summ%100));
  }
  else if (strcmp(argv[1],"tabel") == 0)
  {
    printf ("cont     ; ");
    for (i=0;i<nmon-1;++i) printf ("%10s; ",mon[i]);
    printf ("%10s ",mon[i]);
    printf ("\n");
    for (i=0;i<nde;++i)
    {
      strcpy(nam,del[i]);
      k=0;
      for (j=0;j<7;++j)
      {
        if (nam[j]==0) k=1;
        if (k==1) nam[j]=' ';
      }
      nam[7] = 0;
      printf ("%10s; ",nam);
      for (j=0;j<nmon;++j)
      {
        int cm=0,cme=0;
        for (k=0;k<nrec;++k)
        {
          if ((strcmp(catre[k],nam) == 0) && (strcmp(mon[j],moneda[k]) == 0))
          {
            cm+=suma[k];
          }
          //if (k<10) printf (" || %s , %s || ", dela[k], nam);
          if ((strcmp(dela[k],nam) == 0) && (strcmp(mon[j],moneda[k]) == 0))
          {
            cm-=suma[k];
            dob[i]+=100;
          }
        }
        //printf ("\n");
      sum [j] += cm;
      printf ("%7d.%02d",cm/100,abs(cm%100));
      if (j!=nmon) printf("; ");
      }
      printf("\n");
    }
    printf ("sume      ; ");
    for (i=0;i<nmon;++i) printf ("%7d.%02d; ",sum[i]/100,abs(sum[i]%100));
    printf ("\n");
    printf ("comisioane; \n");
    printf ("conturi   ; ");
    for (i=1;i<nde;++i)
    {
      strcpy(nam,del[i]);
      k=0;
      for (j=0;j<7;++j)
      {
        if (nam[j]==0) k=1;
        if (k==1) nam[j]=' ';
      }
      nam[7] = 0;
      printf ("%10s; ",nam);
    }
    printf ("\n");
    printf ("sume      ; ");
    for (i=1;i<nde;++i) printf ("%7d.%02d; ",dob[i]/100,abs(dob[i]%100));
  }
  else if (strcmp(argv[1],"instrument") == 0)
  {
    printf("WIP\n");
  }
  else if (strcmp(argv[1],"conv") == 0)
  {
    float q=atof(argv[4]);
    char car1[7],car2[7];
    int ki;
    for (ki=0;ki<6-strlen(argv[2]);++ki) car1[ki]=' ';
    for (ki=0;ki<6-strlen(argv[3]);++ki) car2[ki]=' ';
    for (ki=6-strlen(argv[2]);ki<6;++ki) car1[ki]=argv[2][ki+strlen(argv[2])-6];
    for (ki=6-strlen(argv[3]);ki<6;++ki) car2[ki]=argv[3][ki+strlen(argv[3])-6];
    printf ("%8.2f %s = %8.2f %s\n", q, argv[2] , convv(car1,car2,q), argv[3]);
    //printf ("%8.2f %s = %8.2f %s\n", q, argv[2] , q, argv[3]);
  }
  else usage();
  return 0;
}
