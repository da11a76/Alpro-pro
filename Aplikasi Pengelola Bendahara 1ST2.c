#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ANGGOTA 40
#define MAX_TRANSAKSI 500
#define MAX_NAMA 100
#define MAX_KATEGORI 30
#define KAS_PER_BULAN 20000  // Konstanta kas tetap 20k per bulan

// Struktur tanggal
typedef struct { int d, m, y; } Tanggal;
// Struktur transaksi
typedef struct {
    int id;
    char nama[MAX_NAMA];    // anggota atau "(Umum)"
    char tipe[10];          // "kas" atau "keluar"
    int jumlah;
    char kategori[MAX_KATEGORI];
    Tanggal tgl;
    int bulan;              // 1..6 dalam semester
    int semester;           // 1 atau 2
} Transaksi;

// Struktur anggota
typedef struct {
    char nama[MAX_NAMA];
    int bayar[6];           // status bayar tiap bulan
    char label[200];        // label dinamis
    int jumlah_tunggakan;
} Anggota;

// Struktur bendahara
typedef struct {
    char nama[MAX_NAMA];
    char nim[20];
    char kode[20];
    Tanggal berjalan;
    int semesterAktif;
} Bendahara;

// Global data
Anggota anggota1[MAX_ANGGOTA]; int n1;
Anggota anggota2[MAX_ANGGOTA]; int n2;
Transaksi tx[MAX_TRANSAKSI]; int ntx;
Bendahara bh;

// Prototypes
int valid_login();
void loadAnggota();
int cmpTanggal(Tanggal a, Tanggal b);
int validTgl(Tanggal t);
int inSemester(Tanggal t, int sem);
void resetLabel(Anggota *a);
void hitungLabel(Anggota *a, int sem);
void updateAllLabels(int sem);
Anggota* findAnggota(int sem, const char *nama);
void menu();
void inputKas(int sem);
void inputKeluar(int sem);
void listAnggota(int sem);
void cariAnggota(int sem);
void ringkasan(int sem);
void listTransaksi(int sem, int bulan);
void hapusTransaksi(int sem, int bulan);
void bacaStr(char *buf, int max);

int main() {
    loadAnggota();
    if(!valid_login()) return 0;
    menu();
    return 0;
}

// Validasi login bendahara
typedef struct { char *nama, *nim, *kode; } Auth;
int valid_login() {
    Auth a = {"NAILA SANDRA UTAMI","212413707","1ST2BNDTP2025"};
    char buf[MAX_NAMA];
    int valid_nama = 0, valid_nim = 0, valid_kode = 0;

    printf("==== LOGIN BENDAHARA ===\n");

    // Validasi Nama
    while (!valid_nama) {
        printf("Nama: "); fread(buf,1,0,stdin);  // flush
        fgets(buf, MAX_NAMA, stdin); buf[strcspn(buf,"\n")] = '\0';
        if (strcmp(buf, a.nama) != 0)
            printf("Nama salah. Coba lagi.\n");
        else
            valid_nama = 1;
    }

    // Validasi NIM
    while (!valid_nim) {
        printf("NIM: ");
        fgets(buf, MAX_NAMA, stdin); buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, a.nim) != 0)
            printf("NIM salah. Coba lagi.\n");
        else
            valid_nim = 1;
    }

    // Validasi Kode
    while (!valid_kode) {
        printf("Kode Aktivasi: ");
        fgets(buf, MAX_NAMA, stdin); buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, a.kode) != 0)
            printf("Kode salah. Coba lagi.\n");
        else
            valid_kode = 1;
    }

    // Input Tanggal Berjalan
    while (1) {
        printf("Tanggal Berjalan (dd mm yyyy): ");
        if (scanf("%d %d %d", &bh.berjalan.d, &bh.berjalan.m, &bh.berjalan.y) != 3) {
            printf("Input tidak valid. Gunakan format dd mm yyyy.\n");
            while (getchar() != '\n');
            continue;
        }
    if (!validTgl(bh.berjalan) || (!inSemester(bh.berjalan, 1) && !inSemester(bh.berjalan, 2))) {
        printf("Tanggal tidak valid atau tidak termasuk dalam semester aktif.\n");
        continue;
    }
        break;
    }
    getchar(); // bersihkan newline
    // Tentukan semesterAktif berdasar tanggal berjalan
    if (inSemester(bh.berjalan, 1)) {
        bh.semesterAktif = 1;
            } else if (inSemester(bh.berjalan, 2)) {
        bh.semesterAktif = 2;
        } else {
    // (seharusnya tidak sampai ke sini karena sudah divalidasi)
        bh.semesterAktif = 0;
        }

    // Set data login
    printf("Login sukses. Waktu berjalan: %02d-%02d-%04d\n", bh.berjalan.d, bh.berjalan.m, bh.berjalan.y);
    strcpy(bh.nama, a.nama);
    strcpy(bh.nim, a.nim);
    strcpy(bh.kode, a.kode);
    return 1;
}

// Muat data anggota
void loadAnggota() {
    const char *list1[] = {
        "ADINDA ZAKHRA RAMADHANI","ALFIAN ZHIQRI ALFAUZAN","AL-FIAR DI FESA FASA",
        "ALFIYAH REZEQI ROZANAH","AMELIA DERANAYA SHAFIRA","ANANDA PUTRI INDRAWAN",
        "EDIGIO FELIX GODINHO DA COSTA","EMIL SALIM","FAHMI IDRIS SETIAWAN",
        "FARODISILJINAN","FAZL AQMAR KARIM AMRULLAH","FEBRIAN RUBBEN",
        "FIKRI ADI NUGRAHA","GAYUH AURA ALHAQ","GENIVEVA ROSA GUTERRES DE SOUSA BELO",
        "GHAZY RHAYSHIE","I KOMANG VITO WIRYADINATA","IFALIA HALIZAH",
        "KHAIRUNA AMANDA RIYADI","MEI INDRIYANTI SYAMSI","MUHAMMAD ABDUL LATHIF",
        "MUHAMMAD ADITYA UTAMA","MUHAMMAD DAFFA ALIMUDDIN PRATAMA","NABIL MAHASIN",
        "NAILA SANDRA UTAMI","NAKHWA AZZAHRA","NAZHIFUDDIN AKMAL",
        "NISWAH NABILA MUKHLIS","ROMALDA MITA AFOAN BELO","SHEILA BERLIANA SUNAN",
        "SITI NAIRA HASIEFAH HARAHAP","SITTI NAWWALIA SAUMY","SYIFA SHAVILA SURESWARI",
        "TAUFIQ ANANDA DIFKA","VERNINDA INRI MAULIDA RISMA","WAHDANA DJUNAEDI",
        "YEHISKIEL ERWIN TAMBUNAN", NULL
    };
    for(n1=0; list1[n1]; n1++) {
        strncpy(anggota1[n1].nama,list1[n1],MAX_NAMA);
        memset(anggota1[n1].bayar,0,sizeof(anggota1[n1].bayar));
        anggota1[n1].label[0]='\0';
    }
    const char *list2[] = {
        "ALFIAN ZHIQRI ALFAUZAN","ALFIYAH REZEQI ROZANAH","AMELIA DERANAYA SHAFIRA",
        "ANANDA PUTRI INDRAWAN","EDIGIO FELIX GODINHO DA COSTA","EMIL SALIM",
        "FAHMI IDRIS SETIAWAN","FARODISILJINAN","FAZL AQMAR KARIM AMRULLAH",
        "FEBRIAN RUBBEN","FIKRI ADI NUGRAHA","GAYUH AURA ALHAQ",
        "GENIVEVA ROSA GUTERRES DE SOUSA BELO","GHAZY RHAYSHIE","I KOMANG VITO WIRYADINATA",
        "IFALIA HALIZAH","KHAIRUNA AMANDA RIYADI","MEI INDRIYANTI SYAMSI",
        "MUHAMMAD ABDUL LATHIF","MUHAMMAD ADITYA UTAMA","MUHAMMAD DAFFA ALIMUDDIN PRATAMA",
        "NABIL MAHASIN","NAILA SANDRA UTAMI","NAKHWA AZZAHRA",
        "NAZHIFUDDIN AKMAL","NISWAH NABILA MUKHLIS","ROMALDA MITA AFOAN BELO",
        "SHEILA BERLIANA SUNAN","SITI NAIRA HASIEFAH HARAHAP","SITTI NAWWALIA SAUMY",
        "SYIFA SHAVILA SURESWARI","TAUFIQ ANANDA DIFKA","VERNINDA INRI MAULIDA RISMA",
        "WAHDANA DJUNAEDI","YEHISKIEL ERWIN TAMBUNAN", NULL
    };
    for(n2=0; list2[n2]; n2++) {
        strncpy(anggota2[n2].nama,list2[n2],MAX_NAMA);
        memset(anggota2[n2].bayar,0,sizeof(anggota2[n2].bayar));
        anggota2[n2].label[0]='\0';
    }
    ntx=0;
}

int cmpTanggal(Tanggal a, Tanggal b) {
    if(a.y!=b.y) return a.y-b.y;
    if(a.m!=b.m) return a.m-b.m;
    return a.d-b.d;
}
int validTgl(Tanggal t) {
    if(t.y<2024||t.y>2025) return 0;
    if(t.m<1||t.m>12) return 0;
    int md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(t.m==2&&(t.y%400==0||(t.y%4==0&&t.y%100!=0))) md[2]=29;
    if(t.d<1||t.d>md[t.m]) return 0;
    return 1;
}
int inSemester(Tanggal t, int sem) {
    Tanggal s,e;
    if(sem==1) { s=(Tanggal){23,9,2024}; e=(Tanggal){28,2,2025}; }
    else      { s=(Tanggal){3,3,2025};  e=(Tanggal){29,9,2025}; }
    return cmpTanggal(t,s)>=0 && cmpTanggal(t,e)<=0;
}
void resetLabel(Anggota *a) {
    a->label[0] = '\0';
    a->jumlah_tunggakan = 0;
}

void hitungLabel(Anggota *a, int sem) {
    resetLabel(a);
    int tung = 0;
    int bayar_berturut = 0;
    int max_berturut = 0;
    for (int i = 0; i < 6; i++) {
        if (a->bayar[i] == 1) {
            bayar_berturut++;
            if (bayar_berturut > max_berturut) max_berturut = bayar_berturut;
        } else {
            tung++;
            bayar_berturut = 0;
        }
    }
    a->jumlah_tunggakan = tung;

    // Label jika tidak menunggak sama sekali
    if (tung == 0) {
        strcat(a->label, "Si Konsisten Hebat ");
    }
    // Label untuk pembayaran berturut (Elite)
    if (max_berturut >= 3) {
        strcat(a->label, "Elite Global ");
    } else if (max_berturut == 2) {
        strcat(a->label, "Awal Elite Global ");
    }
    // Label untuk tunggakan
    if (tung >= 3) {
        strcat(a->label, "Si Penghutang Handal ");
    } else if (tung == 2) {
        strcat(a->label, "Si Pelupa ");
    }
}

void updateAllLabels(int sem) {
    Anggota *arr = (sem == 1 ? anggota1 : anggota2);
    int n = (sem == 1 ? n1 : n2);
    for (int i = 0; i < n; i++) {
        hitungLabel(&arr[i], sem);
    }
}

Anggota* findAnggota(int sem, const char *nama) {
    Anggota *arr=(sem==1?anggota1:anggota2);
    int n=(sem==1?n1:n2);
    for(int i=0;i<n;i++) if(!strcasecmp(arr[i].nama,nama)) return &arr[i];
    return NULL;
}
void bacaStr(char *buf,int max){ fgets(buf,max,stdin); buf[strcspn(buf,"\n")]='\0'; }

void menu() {
    int choice;
    int sem = bh.semesterAktif;
    while(1) {
        printf("\n+==============================================+\n");
        printf("|      APLIKASI KEUANGAN BENDAHARA KELAS      |\n");
        printf("|              Semester %d               |\n",sem);
        printf("|          Tanggal: %02d-%02d-%04d           |\n",bh.berjalan.d,bh.berjalan.m,bh.berjalan.y);
        printf("|          Kas per bulan: Rp %d,00          |\n", KAS_PER_BULAN);
        printf("+==============================================+\n");
        printf("| 1. Input Kas Masuk                          |\n");
        printf("| 2. Input Pengeluaran                        |\n");
        printf("| 3. Lihat Daftar Anggota & Status            |\n");
        printf("| 4. Ringkasan Keuangan Semester              |\n");
        printf("| 5. Cari Anggota & Tunggakan                 |\n");
        printf("| 6. Lihat Transaksi Bulan & Semester         |\n");
        printf("| 7. Hapus Transaksi                          |\n");
        printf("| 8. Keluar                                   |\n");
        printf("+==============================================+\n");
        printf("Pilih menu [1-8]: ");
        if(scanf("%d",&choice)!=1){ getchar(); continue; }
        getchar();
        switch(choice) {
            case 1: inputKas(sem); break;
            case 2: inputKeluar(sem); break;
            case 3: listAnggota(sem); break;
            case 4: ringkasan(sem); break;
            case 5: cariAnggota(sem); break;
            case 6: {
                int b; printf("Masukkan bulan (1-6): "); scanf("%d",&b); getchar();
                listTransaksi(sem,b); break;
            }
            case 7: {
                int b; printf("Masukkan bulan untuk hapus (1-6): "); scanf("%d",&b); getchar();
                hapusTransaksi(sem,b); break;
            }
            case 8: printf("\nSimpan data dan keluar...\n"); return;
            default: printf("Pilihan tidak valid! Silakan coba lagi.\n");
        }
    }
}

// Fungsi inputKas yang dimodifikasi dengan kas tetap 20k per bulan
void inputKas(int sem) {
    printf("\n== Input Kas Masuk (Semester %d) ==\n", sem);
    printf("** Kas tetap: Rp %d,00 per bulan **\n", KAS_PER_BULAN);

    char nama[MAX_NAMA];
    printf("Nama anggota: ");
    bacaStr(nama, MAX_NAMA);
    Anggota *a = findAnggota(sem, nama);
    if (!a) {
        printf("Anggota tidak ditemukan.\n");
        return;
    }

    // Tampilkan status pembayaran saat ini
    printf("Status pembayaran %s saat ini: ", a->nama);
    for(int i = 0; i < 6; i++) {
        printf("%s%d", (i > 0 ? " " : ""), a->bayar[i]);
    }
    printf(" (1=sudah bayar, 0=belum bayar)\n");

    // Pilih bulan mana yang akan dibayar
    printf("\nPilih bulan yang akan dibayar:\n");
    printf("1. Bayar bulan tertentu\n");
    printf("2. Bayar cicilan berturut-turut dari bulan 1\n");
    printf("Pilihan (1/2): ");

    int pilihan;
    scanf("%d", &pilihan);

    if (pilihan == 1) {
        // Bayar bulan tertentu
        int bulan;
        printf("Masukkan bulan (1-6): ");
        scanf("%d", &bulan);

        if (bulan < 1 || bulan > 6) {
            printf("Bulan tidak valid (1-6).\n");
            return;
        }

        if (a->bayar[bulan-1] == 1) {
            printf("Bulan %d sudah dibayar sebelumnya.\n", bulan);
            return;
        }

        // Konfirmasi pembayaran
        printf("Konfirmasi pembayaran:\n");
        printf("Anggota: %s\n", a->nama);
        printf("Bulan: %d\n", bulan);
        printf("Jumlah: Rp %d,00\n", KAS_PER_BULAN);
        printf("Lanjutkan? (y/n): ");

        char konfirmasi;
        scanf(" %c", &konfirmasi);
        if (konfirmasi != 'y' && konfirmasi != 'Y') {
            printf("Pembayaran dibatalkan.\n");
            return;
        }

        // Input tanggal transaksi
        Tanggal t;
        while (1) {
            printf("Tanggal transaksi (dd mm yyyy): ");
            if (scanf("%d %d %d", &t.d, &t.m, &t.y) != 3) {
                printf("Format tanggal tidak valid. Ulangi lagi.\n");
                while (getchar() != '\n');
                continue;
            }
            if (!validTgl(t) || !inSemester(t, sem) || cmpTanggal(t, bh.berjalan) > 0) {
                printf("Tanggal transaksi tidak valid atau di luar waktu berjalan. Ulangi lagi.\n");
                continue;
            }
            break;
        }
        getchar(); // bersihkan newline

        // Catat transaksi
        Transaksi *p = &tx[ntx++];
        p->id = ntx;
        strcpy(p->nama, a->nama);
        strcpy(p->tipe, "kas");
        p->jumlah = KAS_PER_BULAN;
        sprintf(p->kategori, "Kas Bulan %d", bulan);
        p->tgl = t;
        p->bulan = bulan;
        p->semester = sem;

        // Update status pembayaran
        a->bayar[bulan-1] = 1;

        printf("Kas bulan %d berhasil dicatat.\n", bulan);

    } else if (pilihan == 2) {
        // Bayar cicilan berturut-turut
        int jumlahBulan;
        printf("Jumlah bulan yang akan dibayar (1-6): ");
        scanf("%d", &jumlahBulan);

        if (jumlahBulan < 1 || jumlahBulan > 6) {
            printf("Jumlah bulan tidak valid (1-6).\n");
            return;
        }

        // Cek apakah ada bulan yang sudah dibayar
        int bulanSudahBayar = 0;
        for (int i = 0; i < jumlahBulan; i++) {
            if (a->bayar[i] == 1) {
                bulanSudahBayar = 1;
                break;
            }
        }

        if (bulanSudahBayar) {
            printf("Ada bulan yang sudah dibayar dalam rentang tersebut.\n");
            printf("Gunakan opsi 'Bayar bulan tertentu' untuk bulan yang belum dibayar.\n");
            return;
        }

        // Konfirmasi pembayaran
        printf("Konfirmasi pembayaran:\n");
        printf("Anggota: %s\n", a->nama);
        printf("Bulan: 1 sampai %d\n", jumlahBulan);
        printf("Total: Rp %d,00\n", KAS_PER_BULAN * jumlahBulan);
        printf("Lanjutkan? (y/n): ");

        char konfirmasi;
        scanf(" %c", &konfirmasi);
        if (konfirmasi != 'y' && konfirmasi != 'Y') {
            printf("Pembayaran dibatalkan.\n");
            return;
        }

        // Input tanggal transaksi
        Tanggal t;
        while (1) {
            printf("Tanggal transaksi (dd mm yyyy): ");
            if (scanf("%d %d %d", &t.d, &t.m, &t.y) != 3) {
                printf("Format tanggal tidak valid. Ulangi lagi.\n");
                while (getchar() != '\n');
                continue;
            }
            if (!validTgl(t) || !inSemester(t, sem) || cmpTanggal(t, bh.berjalan) > 0) {
                printf("Tanggal transaksi tidak valid atau di luar waktu berjalan. Ulangi lagi.\n");
                continue;
            }
            break;
        }
        getchar(); // bersihkan newline

        // Catat transaksi untuk setiap bulan
        for (int i = 0; i < jumlahBulan; i++) {
            Transaksi *p = &tx[ntx++];
            p->id = ntx;
            strcpy(p->nama, a->nama);
            strcpy(p->tipe, "kas");
            p->jumlah = KAS_PER_BULAN;
            sprintf(p->kategori, "Kas Bulan %d", i + 1);
            p->tgl = t;
            p->bulan = i + 1;
            p->semester = sem;

            // Update status pembayaran
            a->bayar[i] = 1;
        }

        printf("Kas untuk %d bulan berhasil dicatat.\n", jumlahBulan);

    } else {
        printf("Pilihan tidak valid.\n");
        return;
    }

    // Update label untuk semua anggota
    updateAllLabels(sem);
    printf("Label anggota diperbarui.\n");
}

void inputKeluar(int sem) {
    printf("\n== Input Pengeluaran (Semester %d) ==\n",sem);
    int nominal; char kategori[MAX_KATEGORI]; Tanggal t;
    printf("Nominal pengeluaran: "); scanf("%d",&nominal); getchar();
    printf("Kategori: "); bacaStr(kategori,MAX_KATEGORI);
    printf("Tanggal transaksi (dd mm yyyy): "); scanf("%d %d %d",&t.d,&t.m,&t.y); getchar();
    if(!validTgl(t)||!inSemester(t,sem)||cmpTanggal(t,bh.berjalan)>0){
        printf("Tanggal tidak valid atau di luar waktu berjalan.\n"); return;
    }
    Transaksi *p=&tx[ntx++]; p->id=ntx;
    strcpy(p->nama,"(Umum)"); strcpy(p->tipe,"keluar");
    p->jumlah=nominal; strcpy(p->kategori,kategori);
    p->tgl=t;
    p->bulan = (t.m>=9||t.m<=2? (t.m>=9? t.m-8: t.m+4) : t.m-2);
    p->semester=sem;
    printf("Pengeluaran dicatat.\n");
}

void listAnggota(int sem) {
    Anggota *arr=(sem==1?anggota1:anggota2);
    int n=(sem==1?n1:n2);
    printf("\n===============================================================================\n");
    printf(" No | %-45s | Tunggakan | Total Hutang | Label\n", "Nama Anggota");
    printf("-------------------------------------------------------------------------------\n");
    for(int i=0;i<n;i++){
        int tung=0;
        for(int j=0;j<6;j++) if(!arr[i].bayar[j]) tung++;
        int totalHutang = tung * KAS_PER_BULAN;
        printf("%3d | %-45s |     %d bln |   Rp %6d | %s\n",
            i+1, arr[i].nama, tung, totalHutang, arr[i].label);
    }
    printf("===============================================================================\n");
    printf("* Kas per bulan: Rp %d,00\n", KAS_PER_BULAN);
}

void cariAnggota(int sem) {
    char nama[MAX_NAMA]; Anggota *a;
    do{
        printf("Nama anggota (cari): "); bacaStr(nama,MAX_NAMA);
        a=findAnggota(sem,nama);
        if(!a) printf("Anggota tidak ditemukan.\n");
        else{
            int tung=0;
            printf("\n=== DETAIL ANGGOTA ===\n");
            printf("Nama: %s\n", a->nama);
            printf("Status bayar per bulan: ");
            for(int i=0;i<6;i++){
                printf("Bulan %d:%s ", i+1, a->bayar[i]?"LUNAS":"BELUM");
                if(!a->bayar[i]) tung++;
            }
            int totalHutang = tung * KAS_PER_BULAN;
            printf("\nTunggakan: %d bulan\n", tung);
            printf("Total hutang: Rp %d,00\n", totalHutang);
            printf("Label: %s\n", a->label);
            printf("=====================\n");
        }
        printf("Cari lagi? (y/n): "); char c=getchar(); getchar(); if(c!='y') break;
    }while(1);
}

void ringkasan(int sem) {
    int in[6]={0}, out[6]={0};
    for(int i=0;i<ntx;i++){
        Transaksi *p=&tx[i]; if(p->semester!=sem) continue;
        if(cmpTanggal(p->tgl,bh.berjalan)>0) continue;
        int b=p->bulan-1;
        if(!strcmp(p->tipe,"kas")) in[b]+=p->jumlah;
        else out[b]+=p->jumlah;
    }
    printf("\n== Ringkasan Semester %d ==\n",sem);
    printf("Kas per bulan: Rp %d,00\n", KAS_PER_BULAN);
    printf("==========================================\n");
    int saldo=0;
    for(int i=0;i<6;i++){
        saldo += in[i]-out[i];
        printf("Bulan %d: Masuk=Rp %d, Keluar=Rp %d, Saldo=Rp %d\n",i+1,in[i],out[i],saldo);
    }
    printf("==========================================\n");
    printf("Total Saldo Semester: Rp %d\n", saldo);
}

void listTransaksi(int sem,int bulan) {
    printf("\n== Transaksi Bulan %d - Semester %d ==\n",bulan,sem);
    printf("+----+-------------------------------+--------+---------------------------+-----------+------------+\n");
    printf("| ID | Nama                          | Tipe   | Kategori                  | Jumlah    | Tanggal    |\n");
    printf("+----+-------------------------------+--------+---------------------------+-----------+------------+\n");
    int ti=0,to=0,found=0;
    for(int i=0;i<ntx;i++){
        Transaksi *p=&tx[i]; if(p->semester!=sem||p->bulan!=bulan) continue;
        found=1;
        printf("| %-2d | %-29s | %-6s | %-25s | Rp %-6d | %02d-%02d-%04d |\n",
               p->id,p->nama,
               !strcmp(p->tipe,"kas")?"DEBIT":"KREDIT",
               p->kategori,p->jumlah,p->tgl.d,p->tgl.m,p->tgl.y);
        if(!strcmp(p->tipe,"kas")) ti+=p->jumlah; else to+=p->jumlah;
    }
    if(!found) printf("|        Tidak ada transaksi untuk bulan & semester ini.                             |\n");
    printf("+----+-------------------------------+--------+---------------------------+-----------+------------+\n");
    printf("Total DEBIT  = Rp %d\n",ti);
    printf("Total KREDIT = Rp %d\n",to);
    printf("Saldo bulan  = Rp %d\n",ti-to);
}

void hapusTransaksi(int sem,int bulan) {
    int id;
    do{
        listTransaksi(sem,bulan);
        printf("ID transaksi hapus (0 selesai): "); scanf("%d",&id);
        if(id==0) break;
        int idx=-1;
        for(int i=0;i<ntx;i++) if(tx[i].id==id&&tx[i].semester==sem&&tx[i].bulan==bulan) idx=i;
        if(idx<0){ printf("ID tidak ditemukan.\n"); continue; }

        // Jika yang dihapus adalah transaksi kas, update status bayar anggota
        if(!strcmp(tx[idx].tipe, "kas")) {
            Anggota *a = findAnggota(sem, tx[idx].nama);
            if(a) {
                a->bayar[tx[idx].bulan-1] = 0; // Set kembali ke belum bayar
                printf("Status pembayaran %s bulan %d direset ke belum bayar.\n",
                       a->nama, tx[idx].bulan);
            }
        }

        for(int j=idx;j<ntx-1;j++) tx[j]=tx[j+1];
        ntx--; printf("Transaksi %d dihapus.\n",id);
    }while(1);
    updateAllLabels(sem);
}
