#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_NASABAH 100
#define MAX_PINJAMAN 100
#define MAX_MUTASI 200
#define BUFFER_TIME 3

// Struktur untuk data nasabah
typedef struct{
    char nama[50];
    char password[20];
    float saldo;
}Nasabah;

typedef struct{
    char nama[50];
    float jumlah_pinjaman;
    int status; // 1: ada pinjaman aktif, 0: tidak ada pinjaman aktif
}Pinjaman;

typedef struct{
    char type[50];
    char nama_nasabah[50]; // Menyimpan nama nasabah yang terlibat dalam mutasi
    char barang[50];
    float harga;
}Mutasi;

typedef struct{
    Mutasi data[MAX_MUTASI];
    int top;
}Stack;

// Struktur untuk data pembelian pulsa
typedef struct{
    char nomor_telepon[15];
    float jumlah_nominal;
}Pulsa;

// Inisialisasi stack
void initializeStack(Stack *stack){
    stack->top = -1;
}

// Memeriksa apakah stack kosong
int isEmpty(Stack *stack){
    return stack->top == -1;
}

// Memeriksa apakah stack penuh
int isFull(Stack *stack){
    return stack->top == MAX_MUTASI - 1;
}

// Menambahkan elemen ke dalam stack (push)
void push(Stack *stack, Mutasi data){
    if(!isFull(stack)){
        stack->top++;
        stack->data[stack->top] = data;
    }else{
        printf("Stack penuh.\n");
    }
}

// Menghapus elemen dari stack (pop)
Mutasi pop(Stack *stack){
    Mutasi removedItem;
    if(!isEmpty(stack)){
        removedItem = stack->data[stack->top];
        stack->top--;
        return removedItem;
    }else{
        printf("Stack kosong.\n");
        Mutasi emptyMutasi; // Mengembalikan Mutasi kosong jika stack kosong
        strcpy(emptyMutasi.type, "");
        strcpy(emptyMutasi.barang, "");
        emptyMutasi.harga = 0.0;
        return emptyMutasi;
    }
}

// Mendapatkan elemen teratas dari stack (peek)
Mutasi peek(Stack *stack){
    if(!isEmpty(stack)){
        return stack->data[stack->top];
    }else{
        printf("Stack kosong.\n");
        Mutasi emptyMutasi; // Mengembalikan Mutasi kosong jika stack kosong
        strcpy(emptyMutasi.type, "");
        strcpy(emptyMutasi.barang, "");
        emptyMutasi.harga = 0.0;
        return emptyMutasi;
    }
}

void signUpMenu();

// baca dari file nasabah.txt
int readNasabahFromFile(Nasabah nasabah[]){
    FILE *file = fopen("nasabah.txt", "r");
    if(file == NULL){
        printf("Gagal membuka file untuk pembacaan.\n");
        return 0;
    }

    int count = 0;
    char line[100]; // asumsikan setiap baris memiliki panjang maksimum 100 karakter
    while(fgets(line, sizeof(line), file) != NULL){
        // Memproses setiap baris yang telah dibaca
        // Misalnya, Anda dapat membagi baris menjadi token-token terpisah
        char nama[50], password[20];
        float saldo;
        if(sscanf(line, " %[^#]#%[^#]#%f", nama, password, &saldo) == 3){
            // Jika berhasil, simpan data ke dalam array Nasabah
            strcpy(nasabah[count].nama, nama);
            strcpy(nasabah[count].password, password);
            nasabah[count].saldo = saldo;
            count++;
        }
    }

    fclose(file);
    return count;
}

void writeNasabahToFile(Nasabah nasabah[], int *count){
    FILE *file = fopen("nasabah.txt", "w");
    if(file == NULL){
        printf("Gagal membuka file untuk penulisan.\n");
        return;
    }
    for(int i = 0; i < *count; i++){
        fprintf(file, "%s#%s#%.2f\n", nasabah[i].nama, nasabah[i].password, nasabah[i].saldo);
    }
    fclose(file);
}

// Fungsi untuk menambahkan data nasabah baru ke dalam file
void appendNasabahToFile(Nasabah nasabah){
    FILE *file = fopen("nasabah.txt", "a");
    if(file == NULL){
        printf("Gagal membuka file untuk penulisan.\n");
        return;
    }
    fprintf(file, "%s#%s#%.2f\n", nasabah.nama, nasabah.password, nasabah.saldo);
    fclose(file);
}

// Fungsi untuk membaca data mutasi dari file
void readMutasiFromFile(Stack *mutasiStack){
    FILE *file = fopen("mutasi.txt", "r");
    if(file == NULL){
        printf("Gagal membuka file untuk pembacaan mutasi.\n");
        return;
    }

    char line[100]; // asumsikan setiap baris memiliki panjang maksimum 100 karakter
    while(fgets(line, sizeof(line), file) != NULL){
        // Memproses setiap baris yang telah dibaca
        // Misalnya, Anda dapat membagi baris menjadi token-token terpisah
        char type[50], nama_nasabah[50], barang[50];
        float harga;
        if(sscanf(line, " %[^#]#%[^#]#%[^#]#%f", type, nama_nasabah, barang, &harga) == 4){
            // Jika berhasil, simpan data ke dalam stack mutasi
            Mutasi mutasi;
            strcpy(mutasi.type, type);
            strcpy(mutasi.nama_nasabah, nama_nasabah);
            strcpy(mutasi.barang, barang);
            mutasi.harga = harga;
            push(mutasiStack, mutasi);
        }
    }

    fclose(file);
}

void writeMutasiToFile(Stack *mutasiStack){
    FILE *file = fopen("mutasi.txt", "w");
    if(file == NULL){
        printf("Gagal membuka file untuk penulisan mutasi.\n");
        return;
    }

    // Memproses setiap mutasi dalam stack dan menuliskannya ke file
    Stack tempStack;
    initializeStack(&tempStack);
    while(!isEmpty(mutasiStack)){
        Mutasi mutasi = pop(mutasiStack);
        fprintf(file, "%s#%s#%s#%.2f\n", mutasi.type, mutasi.nama_nasabah, mutasi.barang, mutasi.harga);
        push(&tempStack, mutasi); // Simpan kembali mutasi ke dalam temporary stack
    }

    // Kembalikan isi temporary stack ke stack aslinya
    while(!isEmpty(&tempStack)){
        push(mutasiStack, pop(&tempStack));
    }

    fclose(file);
}

// Fungsi untuk membaca data pinjaman dari file
void readPinjamanFromFile(Pinjaman *pinjaman){
    FILE *file = fopen("pinjaman.txt", "r");
    if(file == NULL){
        printf("Gagal membuka file untuk pembacaan pinjaman.\n");
        return;
    }

    int count = 0;
    char line[100]; // asumsikan setiap baris memiliki panjang maksimum 100 karakter
    while(fgets(line, sizeof(line), file) != NULL && count < MAX_PINJAMAN){
        // Memproses setiap baris yang telah dibaca
        // Misalnya, Anda dapat membagi baris menjadi token-token terpisah
        char nama_nasabah[50];
        float jumlah_pinjaman;
        int status;
        if(sscanf(line, " %[^#]#%f#%d", nama_nasabah, &jumlah_pinjaman, &status) == 3){
            // Jika berhasil, simpan data ke dalam array Pinjaman
            strcpy(pinjaman[count].nama, nama_nasabah);
            pinjaman[count].jumlah_pinjaman = jumlah_pinjaman;
            pinjaman[count].status = status;
            count++;
        }
    }

    fclose(file);
}

void writePinjamanToFile(Pinjaman *pinjaman){
    FILE *file = fopen("pinjaman.txt", "w");
    if(file == NULL){
        printf("Gagal membuka file untuk penulisan pinjaman.\n");
        return;
    }

    // Memproses setiap pinjaman dalam array dan menuliskannya ke file
    for(int i = 0; i < MAX_PINJAMAN; i++){
        fprintf(file, "%s#%.2f#%d\n", pinjaman[i].nama, pinjaman[i].jumlah_pinjaman, pinjaman[i].status);
    }

    fclose(file);
}

int logInMenu(char *nama, char *password, Nasabah nasabah[]){
    printf("--------Log In Menu--------\n");
    printf("Nama: ");
    scanf(" %[^\n]s", nama);
    printf("Password: ");
    scanf(" %[^\n]s", password);

    // Cek kalau ada yang namanya sama
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, nama) == 0 && strcmp(nasabah[i].password, password) == 0){
            return 1; // Return 1 kalau ada yang sama
        }
    }
    return 0; // Return 0 kalau tidak ada
}

void firstMenu(char *NasabahTemp, char *PasswordTemp, Nasabah nasabah[], int *i){
    int WhileLoop = 1;
    int choice = 0;

    while(WhileLoop == 1){
        printf("Main Menu\n");
        printf("1. Log In\n");
        printf("2. Sign Up\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                if(logInMenu(NasabahTemp, PasswordTemp, nasabah)){
                    printf("Login successful.\n");
                    WhileLoop = 0;
                }else{
                    printf("Invalid username or password.\n\n"); // Kalau return 0
                }
                break;
            case 2:
                signUpMenu(nasabah, &*i);
                break;
            default:
                printf("Pilihan tidak valid.\n\n");
        }
    }
}

void signUpMenu(Nasabah nasabah[], int *i){
    if(*i >= MAX_NASABAH){
        printf("Jumlah nasabah sudah mencapai batas maksimum.\n");
        return;
    }

    char namaBaru[50];
    printf("Masukkan nama baru: ");
    scanf(" %[^\n]s", namaBaru);

    // Memeriksa apakah nama akun sudah ada dalam daftar nasabah
    for(int j = 0; j < *i; j++){
        if(strcmp(namaBaru, nasabah[j].nama) == 0){
            printf("Nama akun sudah digunakan. Silakan pilih nama lain.\n\n");
            return;
        }
    }
    strcpy(nasabah[*i].nama, namaBaru);

    printf("Masukkan password: ");
    scanf(" %[^\n]s", nasabah[*i].password);
    nasabah[*i].saldo = 100000;
    printf("Akun %s berhasil dibuat\n\n", nasabah[*i].nama);

    if(*i == 0){
        writeNasabahToFile(nasabah, &*i);
    }else{
        appendNasabahToFile(nasabah[*i]);
    }

    (*i)++;
    return;
}

void gantiPassword(char *NasabahTemp, char *PasswordTemp, Nasabah nasabah[]){
    char temp[MAX_NASABAH];


    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0 && strcmp(nasabah[i].password, PasswordTemp) == 0){
            printf("Ganti Password Akun %s\n", nasabah[i].nama);
            printf("Password Sekarang: ");
            scanf(" %[^\n]s", temp);
            if(strcmp(nasabah[i].password, temp) == 0){
                printf("Set Password Baru: ");
                scanf(" %[^\n]s", nasabah[i].password);
            }else{
                printf("Password salah, silahkan  coba lagi\n\n");
                return;
            }
        }
    }
}

void mutasiTransfer(char *NasabahTemp, char *PasswordTemp, Nasabah nasabah[], Stack *mutasiStack, float transfer, char *nama){
    // Temukan nasabah yang melakukan transfer
    int pengirim_index = -1;
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0 && strcmp(nasabah[i].password, PasswordTemp) == 0){
            pengirim_index = i;
            break;
        }
    }

    // Temukan nasabah penerima
    int penerima_index = -1;
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, nama) == 0){
            penerima_index = i;
            break;
        }
    }

    // Jika kedua nasabah ditemukan
    if(pengirim_index != -1 && penerima_index != -1){
        nasabah[pengirim_index].saldo -= transfer;

        nasabah[penerima_index].saldo += transfer;

        // Tambahkan mutasi pengirim
        Mutasi mutasi_pengirim;
        strcpy(mutasi_pengirim.type, "Pengeluaran");
        strcpy(mutasi_pengirim.nama_nasabah, NasabahTemp); // Simpan nama nasabah pengirim
        strcpy(mutasi_pengirim.barang, "Transfer ke ");
        strcat(mutasi_pengirim.barang, nama);
        mutasi_pengirim.harga = transfer;
        push(mutasiStack, mutasi_pengirim);

        // Tambahkan mutasi penerima
        Mutasi mutasi_penerima;
        strcpy(mutasi_penerima.type, "Pemasukan");
        strcpy(mutasi_penerima.nama_nasabah, nama); // Simpan nama nasabah penerima
        strcpy(mutasi_penerima.barang, "Transfer dari ");
        strcat(mutasi_penerima.barang, NasabahTemp);
        mutasi_penerima.harga = transfer;
        push(mutasiStack, mutasi_penerima);
    }
}

void menuTransfer(char *NasabahTemp, char *PasswordTemp, Nasabah nasabah[], Stack *mutasiStack){
    char nama[MAX_NASABAH];
    float transfer;

    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0 && strcmp(nasabah[i].password, PasswordTemp) == 0){
            printf("==========Menu Transfer==========\n");
            printf("Jumlah Saldo Sekarang: RP %.2f\n", nasabah[i].saldo);
            printf("Transfer Kepada: ");
            scanf(" %[^\n]s", nama);
            printf("Jumlah Yang Ingin Dikirim: Rp ");
            scanf("%f", &transfer);
            if(nasabah[i].saldo < transfer){
                printf("Saldo Tidak Cukup.\n\n");
                return;
            }else{
                for(int j = 0; j < MAX_NASABAH; j++){
                    if(strcmp(nasabah[j].nama, nama) == 0){
                        mutasiTransfer(NasabahTemp, PasswordTemp, nasabah, mutasiStack, transfer, nama);
                        printf("Transfer Rp %.2f ke %s Berhasil.\n\n", transfer, nama);
                        return;
                    }
                }
                printf("Nasabah Tidak Ditemukan.");
                return;
            }
        }
    }
    return;
}

void displayMutasi(Stack *stack, char *nama){
    printf("==========Mutasi Rekening==========\n");
    if(isEmpty(stack)){
        printf("Stack kosong.\n\n");
        return;
    }

    Stack tempStack;
    initializeStack(&tempStack);

    // Memindahkan isi stack ke temporary stack untuk menampilkan dengan urutan yang benar
    while(!isEmpty(stack)){
        Mutasi mutasi = pop(stack);
        if(strcmp(mutasi.nama_nasabah, nama) == 0){ // Filter mutasi berdasarkan nama nasabah yang sedang aktif
            printf("Type: %s\nBarang: %s\nHarga: Rp %.2f\n\n", mutasi.type, mutasi.barang, mutasi.harga);
            push(&tempStack, mutasi); // Memasukkan kembali mutasi yang sudah dipakai ke dalam temporary stack
        }else{
            push(&tempStack, mutasi); // Memasukkan kembali mutasi yang tidak terkait ke dalam temporary stack
        }
    }

    // Memindahkan kembali isi dari temporary stack ke stack aslinya
    while(!isEmpty(&tempStack)){
        push(stack, pop(&tempStack));
    }
}

void displayData(char *nama, Nasabah nasabah[], Pinjaman *pinjaman){

    // Tampilkan data nasabah dan jumlah pinjaman
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, nama) == 0){
            printf("==========Data Nasabah==========\n");
            printf("Nama Akun: %s\n", nama);
            printf("Saldo Sekarang: Rp %.2f\n", nasabah[i].saldo);
        }
    }

    // Hitung jumlah pinjaman yang masih aktif
    for(int i = 0; i < MAX_PINJAMAN; i++){
        if(strcmp(nama, pinjaman[i].nama) == 0 && pinjaman[i].status == 1){
            printf("Jumlah Pinjaman Belum Dibayar: Rp %.2f\n", pinjaman[i].jumlah_pinjaman);
        }
    }
    printf("\n");
    return;
}

void pengajuanPinjaman(char *NasabahTemp, Nasabah *nasabah, Pinjaman *pinjaman, int *pinjamanIndex){
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0){
            if(nasabah[i].saldo <= 0){
                printf("Maaf, saldo Anda tidak mencukupi untuk melakukan pengajuan pinjaman.\n\n");
                return;
            }
        }
    }

    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0){
            if(pinjaman[i].status == 1){
            printf("Maaf, Anda masih memiliki pinjaman yang belum dibayar.\n\n");
            return;
            }
        }
    }

    float jumlah_pinjaman;
    printf("Masukkan jumlah pinjaman yang ingin diajukan: Rp ");
    scanf("%f", &jumlah_pinjaman);

    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0){
            pinjaman[i].jumlah_pinjaman = jumlah_pinjaman;
            pinjaman[i].status = 1; // Mengubah status pinjaman menjadi selesai
        }
    }

    printf("Pengajuan pinjaman sebesar Rp %.2f berhasil diajukan.\n\n", jumlah_pinjaman);

    // Simulasi proses pengajuan
     for(int i = 0; i < BUFFER_TIME; i++){
        printf("Memproses pengajuan pinjaman... %d\n", BUFFER_TIME - i);
        sleep(1); // Menunggu 1 detik
    }
    printf("Pengajuan pinjaman berhasil diproses.\n\n");


    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0){
            // Tambahkan informasi pinjaman ke dalam array pinjaman
            strcpy(pinjaman[i].nama, NasabahTemp); // Simpan nama nasabah yang mengajukan pinjaman
            pinjaman[i].jumlah_pinjaman = jumlah_pinjaman;
            pinjaman[i].status = 1; // Atur status pinjaman menjadi aktif
        }
    }

    // Tambahkan jumlah pinjaman ke saldo akun nasabah yang bersangkutan
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0){
            nasabah[i].saldo += jumlah_pinjaman;
            break;
        }
    }

    (*pinjamanIndex)++;
}

void beliPulsa(char *NasabahTemp, char *PasswordTemp, Nasabah nasabah[], Stack *mutasiStack){
    Pulsa pembelian_pulsa;

    printf("Masukkan nomor telepon: ");
    scanf("%s", pembelian_pulsa.nomor_telepon);
    printf("Masukkan jumlah nominal pulsa yang ingin dibeli: Rp ");
    scanf("%f", &pembelian_pulsa.jumlah_nominal);

    // Menentukan indeks nasabah yang melakukan pembelian
    int nasabah_index = -1;
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0 && strcmp(nasabah[i].password, PasswordTemp) == 0){
            nasabah_index = i;
            break;
        }
    }

    // Jika nasabah ditemukan, kurangi saldo dan tambahkan informasi pembelian pulsa ke stack mutasi
    if(nasabah_index != -1){
        if(nasabah[nasabah_index].saldo < pembelian_pulsa.jumlah_nominal){
            printf("Saldo tidak mencukupi untuk melakukan pembelian pulsa.\n\n");
        }else{
            nasabah[nasabah_index].saldo -= pembelian_pulsa.jumlah_nominal;

            // Tambahkan informasi pembelian pulsa ke stack mutasi
            Mutasi mutasi_pembelian_pulsa;
            strcpy(mutasi_pembelian_pulsa.type, "Pengeluaran");
            strcpy(mutasi_pembelian_pulsa.nama_nasabah, NasabahTemp);
            strcpy(mutasi_pembelian_pulsa.barang, "Pembelian pulsa untuk nomor ");
            strcat(mutasi_pembelian_pulsa.barang, pembelian_pulsa.nomor_telepon);
            mutasi_pembelian_pulsa.harga = pembelian_pulsa.jumlah_nominal;
            push(mutasiStack, mutasi_pembelian_pulsa);

            printf("Pembelian pulsa berhasil.\n\n");
        }
    }else{
        printf("Nasabah tidak ditemukan.\n\n");
    }
}

void beliKuota(char *NasabahTemp, char *PasswordTemp, Nasabah nasabah[], Stack *mutasiStack){
    char provider[50];
    float harga;

    printf("Telkomsel = Rp 50,000.00\n");
    printf("Tri = Rp 75,000.00\n");
    printf("Pilih provider kuota: ");
    scanf("%s", provider);

    // Simpan harga sesuai dengan provider yang dipilih
    if(strcmp(provider, "Telkomsel") == 0){
        harga = 50000;
    }else if(strcmp(provider, "Tri") == 0){
        harga = 75000;
    }else{
        printf("Provider tidak valid.\n\n");
        return;
    }

    // Menentukan indeks nasabah yang melakukan pembelian
    int nasabah_index = -1;
    for(int i = 0; i < MAX_NASABAH; i++){
        if(strcmp(nasabah[i].nama, NasabahTemp) == 0 && strcmp(nasabah[i].password, PasswordTemp) == 0){
            nasabah_index = i;
            break;
        }
    }

    // Jika nasabah ditemukan, kurangi saldo dan tambahkan informasi pembelian kuota ke stack mutasi
    if(nasabah_index != -1){
        if(nasabah[nasabah_index].saldo < harga){
            printf("Saldo tidak mencukupi untuk melakukan pembelian kuota.\n\n");
        }else{
            nasabah[nasabah_index].saldo -= harga;

            // Tambahkan informasi pembelian kuota ke stack mutasi
            Mutasi mutasi_pembelian_kuota;
            strcpy(mutasi_pembelian_kuota.type, "Pengeluaran");
            strcpy(mutasi_pembelian_kuota.nama_nasabah, NasabahTemp);
            strcpy(mutasi_pembelian_kuota.barang, "Pembelian kuota dari ");
            strcat(mutasi_pembelian_kuota.barang, provider);
            mutasi_pembelian_kuota.harga = harga;
            push(mutasiStack, mutasi_pembelian_kuota);

            printf("Pembelian kuota dari %s seharga Rp %.2f berhasil.\n\n", provider, harga);
        }
    }else{
        printf("Nasabah tidak ditemukan.\n\n");
    }
}


int main(){
    char NasabahTemp[MAX_NASABAH];
    char PasswordTemp[MAX_NASABAH];
    int choice = 0;
    int i = 0;
    int WhileLoop = 1;
    int top = -1;
    int pinjamanIndex = 0;

    Nasabah nasabah[MAX_NASABAH];
    Stack mutasiStack;
    initializeStack(&mutasiStack);
    Pinjaman pinjaman[MAX_PINJAMAN];

     // Baca data mutasi dari file
    readMutasiFromFile(&mutasiStack);

    // Baca data pinjaman dari file
    readPinjamanFromFile(pinjaman);

    i = readNasabahFromFile(nasabah); // return nilai sebanyak line di file nasabah.txt
    firstMenu(NasabahTemp, PasswordTemp, nasabah, &i);

    while(WhileLoop == 1){
        printf("============Menu============\n");
        printf("1. Tampilkan Data Nasabah\n");
        printf("2. Transfer Antar Nasabah\n");
        printf("3. Mutasi Rekening\n");
        printf("4. Ganti Password Akun\n");
        printf("5. Pengajuan Pinjaman\n");
        printf("6. Menu Beli Pulsa\n");
        printf("7. Menu Beli Kuota\n");
        printf("8. Informasi Promosi\n");
        printf("9. Ganti Akun\n");
        printf("10. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                displayData(NasabahTemp, nasabah, pinjaman);
                break;
            case 2:
                menuTransfer(NasabahTemp, PasswordTemp, nasabah, &mutasiStack);
                break;
            case 3:
                displayMutasi(&mutasiStack, NasabahTemp);
                break;
            case 4:
                gantiPassword(NasabahTemp, PasswordTemp, nasabah);
                break;
            case 5:
                pengajuanPinjaman(NasabahTemp, nasabah, pinjaman, &pinjamanIndex);
                break;
            case 6:
                beliPulsa(NasabahTemp, PasswordTemp, nasabah, &mutasiStack);
                break;
            case 7:
                beliKuota(NasabahTemp, PasswordTemp, nasabah, &mutasiStack);
                break;
            case 8:
                printf("1. Promosi Diskon Pulsa\n");
                printf("Nikmati diskon spesial untuk pembelian pulsa! Dapatkan potongan harga hingga 20%% \nuntuk setiap pembelian pulsa di aplikasi kami. Jangan lewatkan kesempatan ini untuk hemat \npengeluaran dan tetap terhubung dengan orang-orang terkasih.\n\n");
                printf("2. Promosi Diskon Kuota Internet\n");
                printf("Habis kuota internet? Tenang, kami punya solusi! Beli kuota internet sekarang dan dapatkan \ndiskon menarik hingga 30%%! Nikmati koneksi internet yang cepat dan stabil dengan harga \nyang lebih terjangkau.\n\n");
                break;
            case 9:
                firstMenu(NasabahTemp, PasswordTemp, nasabah, &i);
                break;
            case 10:
                WhileLoop = 0;
                writeNasabahToFile(nasabah, &i); // Tulis data nasabah ke file sebelum keluar dari program
                writeMutasiToFile(&mutasiStack); // Tulis data mutasi ke file sebelum keluar dari program
                writePinjamanToFile(pinjaman); // Tulis data pinjaman ke file sebelum keluar dari program
                break;
            default:
                printf("Pilihan tidak valid.\n\n");
                break;
        }
    }
    return 0;
}
