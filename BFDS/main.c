#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

/*Structs*/
//------------------------------------------------------------------------//
typedef struct Currency{

    char Name[20];
    int quantity;

}Currency, *CuPointer;

typedef struct DataQuantity{
    
    int Clients;
    int Coins;

}DataQuantity;

typedef struct Coin{

    char Name[20];
    double Value;
    double SellTax;
    double BuyTax;

}Coin, *CoPointer;

typedef struct Extract{
    
    char Name[20];
    char Cpf[12];
    char TransactionType;
    //'+'(pra compra) ou '-'(pra venda)
    Currency Currency;
    Coin Coin;
    int IDNumber;
    struct tm Date;

}Extract;

typedef struct Client{

    bool IsAdm;

    char Name[20];
    char Cpf[12];
    char Pass[7];

    int CurrentExtractIndex;
    //isso daqui não é pra mexer é só da função de extrato

    CuPointer Currencies;
    Extract Extract[100];

}Client, *ClPointer;
//------------------------------------------------------------------------//



/*Prototipos*/
//------------------------------------------------------------------------//

void debugCoin(CoPointer pCoins, DataQuantity dataQuantity, int index);

void debugClient(ClPointer ClPointer, DataQuantity dataQuantity, int index);


//------------------------------------------------------------------------//



/*Main*/
//------------------------------------------------------------------------//
int main(){
    
    /*Declarações de variavies*/
    //------------------------------------------------------------------------//
    setlocale(LC_ALL, "portuguese");
    setenv("TZ","America/Sao_Paulo",1);
    tzset();
    srand(time(NULL));

    FILE *dBDataQuantities, *dBCoins, *dBClients;
    const char *dataQuantities = "DataQuantity.bin", *coins = "Coin.bin", *clients = "Clients.bin";


    DataQuantity dataQuantity;
    dataQuantity.Coins = -1;
    dataQuantity.Clients = -1;


    CoPointer pCoins = NULL;
    
    ClPointer pClients = NULL;


    int loggedClientIndex = -1;
    ClPointer loggedClient = NULL;

    char UserResponse[20];
    char lineRead[256];
    //------------------------------------------------------------------------//


    
    /*Atribuções das variavies*/
    //------------------------------------------------------------------------//
    dBDataQuantities = fopen(dataQuantities, 'r');
    if(dBDataQuantities == NULL){
        perror("falha ao abrir \"DataQuantity.bin\"");
        return 1;
    }     
    fgets(lineRead, sizeof(lineRead), dBDataQuantities);
    fgets(lineRead, sizeof(lineRead), dBDataQuantities);
    sscanf(lineRead, "%d,%d", &dataQuantity.Coins, &dataQuantity.Clients);
    fclose(dBDataQuantities);
    if(dataQuantity.Coins < 0 || dataQuantity.Clients < 0){
        perror("falha, \"dataQuantity.Coins\" e \"dataQuantity.Clients\" tem que ser maior ou igual a zero");
        return 3;
    }

    pCoins = (CoPointer)calloc(dataQuantity.Coins, sizeof(Coin));
    if(pCoins == NULL){
        perror("falha ao alocar memoria para \"pCoins\"");
        return 2;
    }
    dBCoins = fopen(coins, "rb");
    if(dBCoins == NULL){
        perror("falha ao abrir \"Coin.bin\"");
        return 1;
    } 
    fread(pCoins, sizeof(Coin), dataQuantity.Coins, dBCoins);
    fclose(dBCoins);

    pClients = (ClPointer)calloc(dataQuantity.Clients, sizeof(Client));
    if(pClients == NULL){
        perror("falha ao alocar memoria para \"pClients\"");
        return 2;
    }
    for (int i = 0; i < dataQuantity.Clients; i++){
        pClients[i].Currencies = (CuPointer)calloc(dataQuantity.Coins, sizeof(Coin));
        if(pClients[i].Currencies == NULL){
            perror("falha ao alocar memoria para \"pClients[i].Currencies\"");
            return 2;
        }
    }  
    dBClients = fopen(clients, "rb");
    if(dBClients == NULL){
        perror("falha ao abrir \"Clients.bin\"");
        return 1;
    } 
    fread(pClients, sizeof(pClients[0]), dataQuantity.Clients, dBClients);
    fclose(dBClients);
    //------------------------------------------------------------------------//
    
    
    
    /*Principal do usuario*/
    //------------------------------------------------------------------------//

    //------------------------------------------------------------------------//
    free(pCoins);
    FreePClients(pClients, dataQuantity);
}
//------------------------------------------------------------------------//



/*Print/DebugFunctions*/
//------------------------------------------------------------------------//

void debugCoin(CoPointer pCoins, DataQuantity dataQuantity, int index){

    if(index == -1){ //Todos as Coins
        for(index = 0; index < dataQuantity.Coins; index++){
        
        printf("===============================\n");
        printf("Nome: %s\n", pCoins[index].Name);
        printf("Valor: %s\n", pCoins[index].Value);
        printf("Taxa de Venda: %lf\n", pCoins[index].SellTax);
        printf("Taxa de Compra: %lf\n", pCoins[index].BuyTax);
        printf("===============================\n");
        }
    }
    else if(index > -1 && index < dataQuantity.Coins){ // Coin de Index Específico
        
        printf("===============================\n");
        printf("Nome: %s\n", pCoins[index].Name);
        printf("Valor: %s\n", pCoins[index].Value);
        printf("Taxa de Venda: %lf\n", pCoins[index].SellTax);
        printf("Taxa de Compra: %lf\n", pCoins[index].BuyTax);
        printf("===============================\n");
    }
    else{
        printf("Index inválido na função \"debugCoin\".\n");
    }
}

void debugClient(ClPointer pClients, DataQuantity dataQuantity, int index){
    
    if(index == -1){ // Todos os Users
        //Todos
        for(index = 0; index < dataQuantity.Clients; index++){
            printf("===============================\n");
            printf("IsAdm: %s", pClients[index].IsAdm ? "True" : "False");
            printf("Nome: %s\n", pClients[index].Name);
            printf("CPF: %s\n", pClients[index].Cpf);
            printf("Senha: %s\n", pClients[index].Pass);
            for(int j = 0; j < dataQuantity.Coins ; j++){
                printf("%s : %lf \n", pClients[index].Currencies[j].Name, pClients[index].Currencies[j].quantity);
            printf("===============================\n");

            printf("Extratos:\n");

            for(int IndexExtract = 0; IndexExtract <100; IndexExtract++){
                printf("||Nome: %s || CPF: %s || Tipo de Transação: %s || Nome da Moeda: %s || Quantidade Movida: %lf || Moeda: %s "
                "|| Moeda: %s || Valor da Moeda: %lf || Taxa de Venda: %lf || Taxa de Compra: %lf || ID do Extrato: %d || "
                "Horário: %d:%d:%d\n", 
                pClients[index].Extract[IndexExtract].Name, pClients[index].Extract[IndexExtract].Cpf, 
                pClients[index].Extract[IndexExtract].TransactionType, pClients[index].Extract[IndexExtract].Currency.Name,
                pClients[index].Extract[IndexExtract].Currency.quantity, pClients[index].Extract[IndexExtract].Coin.Name,
                pClients[index].Extract[IndexExtract].Coin.Value, pClients[index].Extract[IndexExtract].Coin.SellTax,
                pClients[index].Extract[IndexExtract].Coin.BuyTax, pClients[index].Extract[IndexExtract].IDNumber, 
                pClients[index].Extract[IndexExtract].Date);
            }
            // Remover Nome e CPF, Quantitiy será alterada
            printf("===============================\n");
            }
        }
    }
    else if(index > -1 && index < dataQuantity.Clients){ // Users de Index Específico

        for(index = 0; index < dataQuantity.Clients; index++){
            printf("_______________________________\n");
            printf("IsAdm: %s", pClients[index].IsAdm ? "True" : "False");
            printf("Nome: %s\n", pClients[index].Name);
            printf("CPF: %s\n", pClients[index].Cpf);
            printf("Senha: %s\n", pClients[index].Pass);
            for(int j = 0; j < dataQuantity.Coins ; j++){
                printf("%s : %lf \n", pClients[index].Currencies[j].Name, pClients[index].Currencies[j].quantity);
            
            printf("===============================\n");

            printf("Extratos:\n");

            for(int IndexExtract = 0; IndexExtract < 100; IndexExtract++){
                printf("||Nome: %s || CPF: %s || Tipo de Transação: %s || Nome da Moeda: %s || Quantidade Movida: %lf || Moeda: %s "
                "|| Moeda: %s || Valor da Moeda: %lf || Taxa de Venda: %lf || Taxa de Compra: %lf || ID do Extrato: %d || "
                "Horário: %d:%d:%d\n", 
                pClients[index].Extract[IndexExtract].Name, pClients[index].Extract[IndexExtract].Cpf, 
                pClients[index].Extract[IndexExtract].TransactionType, pClients[index].Extract[IndexExtract].Currency.Name,
                pClients[index].Extract[IndexExtract].Currency.quantity, pClients[index].Extract[IndexExtract].Coin.Name,
                pClients[index].Extract[IndexExtract].Coin.Value, pClients[index].Extract[IndexExtract].Coin.SellTax,
                pClients[index].Extract[IndexExtract].Coin.BuyTax, pClients[index].Extract[IndexExtract].IDNumber, 
                pClients[index].Extract[IndexExtract].Date);
            }
            // Remover Nome e CPF, Quantitiy será alterada
            printf("===============================\n");
            }
        }
    }
    else{
        printf("Index inválido na função \"debugClient\".\n");
    }
}

//------------------------------------------------------------------------//



/*MemoryFunctions*/
//------------------------------------------------------------------------//
void SaveDataQuantity(DataQuantity dataQuantity, const char *dataQuantities){
    FILE* destino;
    destino = fopen(dataQuantities, 'w');
    if(destino == NULL){
        perror("falha ao abrir o arquivo destino no \"SaveDataQuantity\"");
    }
    else{
        fprintf(destino, "Coins,Clients\n");
        fprintf(destino, "%d,%d", dataQuantity.Coins, dataQuantity.Clients);
        fclose(destino);
    }
}

void SaveCoin(CoPointer pCoins, int coinsQuantity, const char *coins){
    FILE* destino;
    destino = fopen(coins, "wb");
    if(destino == NULL){
        perror("falha ao abrir o arquivo destino no \"SaveCoin\"");
    }
    else{
        fwrite(pCoins, sizeof(Coin), coinsQuantity, destino);
        fclose(destino);
    }
}

void SaveClient(ClPointer pClients, int clientsQuantity, const char *clients){
    FILE* destino;
    destino = fopen(clients, "wb");
    if(destino == NULL){
        perror("falha ao abrir o arquivo destino no \"SaveClient\"");
    }
    else{
        fwrite(pClients, sizeof(pClients[0]), clientsQuantity, destino);
        fclose(destino);
    }
}

void FreePClients(ClPointer pClients, DataQuantity dataQuantity){
    for(int i = 0; i < dataQuantity.Clients; i++){
        for(int j = 0; j < dataQuantity.Coins; i++){
            free(pClients[i].Currencies);
            pClients[i].Currencies == NULL;
        }
    }
    free(pClients);
    pClients == NULL;
}

bool AddCoin(CoPointer pCoins, DataQuantity *dataQuantity, char *name, double value, double sellTax, double buyTax){
    dataQuantity[0].Coins++;
    pCoins = (CoPointer)realloc(pCoins, sizeof(Coin) * dataQuantity[0].Coins);
    //memset(&pCoins[dataQuantity[0].Coins-1], 0, sizeof(Coin)); suponho que não precisa por ser uma estrutura simples
    strcpy(pCoins[dataQuantity[0].Coins-1].Name, name);
    pCoins[dataQuantity[0].Coins-1].Value = value;
    pCoins[dataQuantity[0].Coins-1].SellTax = sellTax;
    pCoins[dataQuantity[0].Coins-1].BuyTax = buyTax;
}

void AddClient(ClPointer pClients, DataQuantity *dataQuantity, bool isAdm, char *name, char *cpf, char *pass){
    //precisa existir ao menos um cliente para essa função funcionar pois ela usa o tamanho do client[0] como referencia aos outros
    dataQuantity[0].Clients++;
    (ClPointer)realloc(pClients, sizeof(pClients[0]) * dataQuantity[0].Clients);
    memset(&pClients[dataQuantity[0].Clients-1], 0, sizeof(pClients[0]));
    pClients[dataQuantity[0].Clients-1].IsAdm = isAdm;
    strcpy(pClients[dataQuantity[0].Clients-1].Name, name);
    strcpy(pClients[dataQuantity[0].Clients-1].Cpf, cpf);
    strcpy(pClients[dataQuantity[0].Clients-1].Pass, pass);
}

bool RemoveCoin(CoPointer pCoins, int indexToRemove, DataQuantity *dataQuantity){
    CoPointer tempPCoins;
    tempPCoins = (CoPointer)calloc(sizeof(Coin), dataQuantity[0].Coins-1);
    if(tempPCoins == NULL){
        perror("erro ao alocar memoria para \"tempPCoins\" em \"AddCoin\"");
        return false;
    }
    int j = 0;
    for (int i = 0; i < dataQuantity[0].Coins; i++){
        if(indexToRemove != i){
            tempPCoins[j] = pCoins[i];
            j++;
        }
    }
    dataQuantity[0].Coins--;
    pCoins = (CoPointer)realloc(pCoins, sizeof(Coin) * dataQuantity[0].Coins);
    for(int i = 0; i < dataQuantity[0].Coins; i++){
        pCoins[i] = tempPCoins[i]; 
    }
    free(tempPCoins);
    return true;
}

bool RemoveClient(ClPointer pClients, int indexToRemove, DataQuantity *dataQuantity){
    ClPointer tempPClients;
    tempPClients = (ClPointer)calloc(sizeof(pClients[0]), dataQuantity[0].Clients-1);
    if(tempPClients == NULL){
        perror("erro ao alocar memoria para \"tempPClients\" em \"RemoveClient\"");
        return false;
    }
    int j = 0;
    for (int i = 0; i < dataQuantity[0].Clients; i++){
        if(indexToRemove != i){
            tempPClients[j] = pClients[i];
            j++;
        }
        else{
            free(pClients[i].Currencies);
        }
    }
    dataQuantity[0].Clients--;
    pClients = (ClPointer)realloc(pClients, sizeof(pClients[0]) * dataQuantity[0].Clients);
    for(int i = 0; i < dataQuantity[0].Clients; i++){
        pClients[i] = tempPClients[i];
    }
    free(tempPClients);
    return true;
}
//------------------------------------------------------------------------//