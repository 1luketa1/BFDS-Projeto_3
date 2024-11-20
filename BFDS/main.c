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
    double quantity;

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
    
    char TransactionType;
    //'D'(pra deposito), 'S'(pra venda) ou 'B' (Pra compra)
    Coin Coin;
    double Quantity;
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
void printExtrato(ClPointer pClients, DataQuantity dataQuantities, int index);
void debugCoin(CoPointer pCoins, DataQuantity dataQuantity, int index);
void debugClient(ClPointer ClPointer, DataQuantity dataQuantity, int index);
void SaveDataQuantity(DataQuantity dataQuantity, const char *dataQuantities);
void SaveCoin(CoPointer pCoins, int coinsQuantity, const char *coins);
void SaveClient(ClPointer pClients, int clientsQuantity, const char *clients);
void FreePClients(ClPointer pClients, DataQuantity dataQuantity);
void AddCoin(ClPointer *pClients, CoPointer *pCoins, DataQuantity *dataQuantity, char *name, double value, double sellTax, double buyTax);
void AddClient(CoPointer pCoins, ClPointer *pClients, DataQuantity *dataQuantity, bool isAdm, char *name, char *cpf, char *pass);
bool RemoveCoin(ClPointer pClients, CoPointer *pCoins, int indexToRemove, DataQuantity *dataQuantity);
bool RemoveClient(ClPointer *pClients, int indexToRemove, DataQuantity *dataQuantity);
void AddExtract(ClPointer *pClients, int clientIndex, char transactionType, Coin coin, int quantity);
void limparTerminal();
void MenuInicial();
bool Login(ClPointer pClients, int clientsQuantity,int *indexClient);
void Passar();
void limparBufferEntrada();
void MenuAdm(ClPointer pClients, int userIndex);
void MenuInvestidor(ClPointer pClients, int userIndex);
void AtualizarCotacoes(CoPointer *pCriptos, int criptosQuantity);
void VenderCriptomoedas(ClPointer *pClients, int userIndex, CoPointer pCriptos, DataQuantity dataQuantities);
void ComprarCriptomoedas(ClPointer *pClients, int userIndex, CoPointer pCriptos, DataQuantity dataQuantities);
void SacarReais(ClPointer *pClients, int userIndex, CoPointer pCriptos);
void DepositarReais(ClPointer *pClients, int userIndex, CoPointer pCriptos);
void ConsultarSaldo(ClPointer pClients, int userIndex, DataQuantity dataQuantities);
int PedirSenha(char userSenhaCerta[7]);
void ConsultarExtratoInvestidor(ClPointer pClients, DataQuantity dataQuantities);
void ConsultarSaldoInvestidor(ClPointer pClients, DataQuantity dataQuantities);
int isdigit( int arg );
void ConsultarExtratoInvestidor(ClPointer pClients, DataQuantity dataQuantities);
void ConsultarSaldoInvestidor(ClPointer pClients, DataQuantity dataQuantities);
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

    // dataQuantity.Coins = 0;
    // dataQuantity.Clients = 0;
    // AddCoin(&pClients, &pCoins, &dataQuantity, "reais", 1,0,0);
    // SaveDataQuantity(dataQuantity, dataQuantities);
    // SaveCoin(pCoins, dataQuantity.Coins, coins);


    
    /*Atribuções das variavies*/
    //------------------------------------------------------------------------//
    dBDataQuantities = fopen(dataQuantities, "r");
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
    char respostaUserPP;
    int indexClient;
    while(true){

        limparTerminal();
        //Menu principal
        MenuInicial();
        //Pergunta se o usuario que se cadastrar ou logar
        scanf(" %c", &respostaUserPP);

        //Cadastro
        if(respostaUserPP == '1'){
            limparTerminal();
                bool IsAdm, tudoCerto;
                char name[20], cpf[12], pass[7], respostaadm;

                while (true) {
                    printf("Seu nome: ");
                    scanf(" %19s", name); 

                    printf("Seu CPF: ");
                    scanf(" %11s", cpf);

                    tudoCerto = true;
                    for (int i = 0; cpf[i] != '\0'; i++) {
                        if (!isdigit(cpf[i])) {
                            printf("O CPF é composto somente de números!\n");
                            tudoCerto = false;
                            Passar();
                            break;
                        }
                    }

                    if (tudoCerto) {
                        printf("Sua senha (6 dígitos): ");
                        scanf(" %6s", pass);

                        tudoCerto = true;
                        for (int i = 0; pass[i] != '\0'; i++) {
                            if (!isdigit(pass[i])) {
                                printf("A senha é composta somente de números!\n");
                                tudoCerto = false;
                                Passar();
                                break;
                            }
                        }

                        if (tudoCerto) {
                            printf("Você é ADM (S/n)? ");
                            scanf(" %c", &respostaadm); 

                            if (respostaadm == 's' || respostaadm == 'S') {
                                IsAdm = true;
                            } else {
                                IsAdm = false;
                            }

                            break;
                        }
                    }
                }

                AddClient(pCoins, &pClients, &dataQuantity, IsAdm, name, cpf, pass);
                SaveClient(pClients, dataQuantity.Clients, clients);
        }
        //Login
        else if(respostaUserPP == '2'){
            limparTerminal();
            Login(pClients, dataQuantity.Clients, &indexClient);

            if(pClients[indexClient].IsAdm){
                MenuAdm(pClients, indexClient);
                scanf(" %c", &respostaUserPP);

                if(respostaUserPP == '1'){
                    bool IsAdm, tudoCerto;
                char name[20], cpf[12], pass[7], respostaadm;

                while (true) {
                    printf("Nome do investidor: ");
                    scanf(" %19s", name); 

                    printf("CPF do investidor: ");
                    scanf(" %11s", cpf);

                    tudoCerto = true;
                    for (int i = 0; cpf[i] != '\0'; i++) {
                        if (!isdigit(cpf[i])) {
                            printf("O CPF é composto somente de números!\n");
                            tudoCerto = false;
                            Passar();
                            break;
                        }
                    }

                    if (tudoCerto) {
                        printf("Senha do investidor (6 dígitos): ");
                        scanf(" %6s", pass);

                        tudoCerto = true;
                        for (int i = 0; pass[i] != '\0'; i++) {
                            if (!isdigit(pass[i])) {
                                printf("A senha é composta somente de números!\n");
                                tudoCerto = false;
                                Passar();
                                break;
                            }
                        }

                        if (tudoCerto) {
                            printf("ADM (S/n)? ");
                            scanf(" %c", &respostaadm); 

                            if (respostaadm == 's' || respostaadm == 'S') {
                                IsAdm = true;
                            } else {
                                IsAdm = false;
                            }

                            break;
                        }
                    }
                }

                AddClient(pCoins, &pClients, &dataQuantity, IsAdm, name, cpf, pass);
                SaveClient(pClients, dataQuantity.Clients, clients);

                }

                if(respostaUserPP == '2'){
                    char cpfremove[12], confirma;
                    bool userAchado;

                    for ( int indexPlayerLoop = 0; indexPlayerLoop < dataQuantity.Clients; indexPlayerLoop++) {
                        //Verifica se o CPF e senha são os mesmos para aquele index
                        if (strcmp(pClients[indexPlayerLoop].Cpf, cpfremove) == 0) {
                            //caso a condição seja verdadeira, ele faz userAchado = false
                            userAchado = true;
                            break;
                        }
                    }
                    if(userAchado){
                        printf("Quer mesmo excluir(s/n)?\n");
                        scanf("%c", confirma);
                        if(confirma == 's' || confirma == 'S'){
                            RemoveClient(pClients, indexClient, dataQuantities);
                        }
                        else if(confirma == 'n' || confirma == 'N'){
                            printf("Você negou a confirmação!\n");
                            Passar();
                        }
                        else{
                            printf("Entrada inválida, operação cancelada. \n");
                            Passar();
                        }
                    }
                    else{
                        printf("User não encontrado!\n");
                        Passar();
                    }
                            
                }
                if(respostaUserPP == '3'){
                    char nome[20];
                    int valor, selltax,buytax;

                    printf("Nome da moeda: ");
                    scanf(" %s", nome);
                    printf("Valor: ");
                    scanf(" %lf", valor);
                    printf("Selltax: ");
                    scanf(" %lf", selltax);
                    printf("buytax: ");
                    scanf(" %lf", buytax);


                    AddCoin(&pClients, &pCoins,&dataQuantities,nome,valor,selltax,buytax);

                }
                if(respostaUserPP == '4'){







                }
                if(respostaUserPP == '5'){
                    ConsultarSaldoInvestidor(pClients, dataQuantity);

                }


                if(respostaUserPP == '6'){
                    ConsultarExtratoInvestidor(pClients, dataQuantity);
                }

                if(respostaUserPP == '7'){
                    //AtualizarCotacoes(pCriptos, );
                }
                if(respostaUserPP == '8'){
                    break;
                }
                else{
                    printf("Valor inválido!\n");
                    Passar();
                }

            }
            else{
                MenuInvestidor(pClients, indexClient);
                scanf(" %c", &respostaUserPP);

                if(respostaUserPP == '1') ConsultarSaldo(pClients, indexClient, dataQuantity);
                else if(respostaUserPP == '2') printExtrato(pClients, dataQuantity, indexClient);
                else if(respostaUserPP == '3') DepositarReais(&pClients, indexClient, pCripto);
                else if(respostaUserPP == '4') SacarReais(&pClients, indexClient, pCriptos);
                else if(respostaUserPP == '5') ComprarCriptomoedas(&pClients, indexClient, pCriptos, dataQuantity);
                else if(respostaUserPP == '6') VenderCriptomoedas(&pClients, indexClient, pCriptos, dataQuantity);
                else if(respostaUserPP == '7') AtualizarCotacoes(pCriptos, dataQuantity.Coins);
                else if(respostaUserPP == '8') {
                    break;
                }



            }

        }
        //Caso não seja 1 ou 2
        else{
            limparTerminal();
            printf("Input inválido, tente novamente...");
            Passar();
        }
    }
    //------------------------------------------------------------------------//
    free(pCoins);
    FreePClients(pClients, dataQuantity);
}
//------------------------------------------------------------------------//



/*Print/DebugFunctions*/
//------------------------------------------------------------------------//
void printExtrato(ClPointer pClients, DataQuantity dataQuantities, int index){

    if(index >= dataQuantities.Clients || index < 0){
        printf("Índice inválido!\n");
        return;
    }
    else{
            printf("Nome: %s\n", pClients[index].Name);
            printf("CPF: %s\n", pClients[index].Cpf);

            printf("===============================\n");
            printf("Extratos:\n");

            for(int IndexExtract = 0; IndexExtract < 100; IndexExtract++){
                printf("|| Tipo de Transação: %c || Nome da Moeda: %s || Valor da Moeda: %.2lf || Taxa de Venda: %.2lf "
                   "|| Taxa de Compra: %.2lf || ID do Extrato: %d || Quantidade Movida: %.2lf || "
                   "Horário: %02d:%02d:%02d ||\n",
                   pClients[index].Extract[IndexExtract].TransactionType, 
                   pClients[index].Extract[IndexExtract].Coin.Name, 
                   pClients[index].Extract[IndexExtract].Coin.Value, 
                   pClients[index].Extract[IndexExtract].Coin.SellTax,
                   pClients[index].Extract[IndexExtract].Coin.BuyTax,
                   pClients[index].Extract[IndexExtract].IDNumber,
                   pClients[index].Extract[IndexExtract].Quantity, 
                   pClients[index].Extract[IndexExtract].Date.tm_hour,
                   pClients[index].Extract[IndexExtract].Date.tm_min, 
                   pClients[index].Extract[IndexExtract].Date.tm_sec);
            }
            printf("===============================\n");  
    }
}

void debugClient(ClPointer pClients, DataQuantity dataQuantities, int index){
    
    if(index == -1){ // Todos os Users
        //Todos
        for(index = 0; index < dataQuantities.Clients; index++){
            printf("===============================\n");
            printf("IsAdm: %s", pClients[index].IsAdm ? "True" : "False");
            printf("Nome: %s\n", pClients[index].Name);
            printf("CPF: %s\n", pClients[index].Cpf);
            printf("Senha: %s\n", pClients[index].Pass);
            for(int j = 0; j < dataQuantities.Coins ; j++){
                printf("%s : %lf \n", pClients[index].Currencies[j].Name, pClients[index].Currencies[j].quantity);
            }
            printf("===============================\n");

            printExtrato(pClients, dataQuantities, index);

            printf("===============================\n");
        }
    }
    else if(index > -1 && index < dataQuantities.Clients){ // Users de Index Específico
        printf("===============================\n");
        printf("IsAdm: %s", pClients[index].IsAdm ? "True" : "False");
        printf("Nome: %s\n", pClients[index].Name);
        printf("CPF: %s\n", pClients[index].Cpf);
        printf("Senha: %s\n", pClients[index].Pass);
        for(int j = 0; j < dataQuantities.Coins ; j++){
            printf("%s : %lf \n", pClients[index].Currencies[j].Name, pClients[index].Currencies[j].quantity);
        }   
        printf("===============================\n");

        printExtrato(pClients, dataQuantities, index);

        printf("===============================\n");
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
    destino = fopen(dataQuantities, "w");
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

void AddCoin(ClPointer *pClients, CoPointer *pCoins, DataQuantity *dataQuantity, char *name, double value, double sellTax, double buyTax){
    dataQuantity[0].Coins++;
    (*pCoins) = (CoPointer)realloc((*pCoins), sizeof(Coin) * dataQuantity[0].Coins);
    //memset(&pCoins[dataQuantity[0].Coins-1], 0, sizeof(Coin)); suponho que não precisa por ser uma estrutura simples
    strcpy((*pCoins)[dataQuantity[0].Coins-1].Name, name);
    (*pCoins)[dataQuantity[0].Coins-1].Value = value;
    (*pCoins)[dataQuantity[0].Coins-1].SellTax = sellTax;
    (*pCoins)[dataQuantity[0].Coins-1].BuyTax = buyTax;
    for(int i = 0; i < dataQuantity[0].Clients; i++){
        (*pClients)[i].Currencies = (CuPointer)realloc((*pClients)[i].Currencies, dataQuantity[0].Coins * sizeof(Currency));
        strcpy((*pClients)[i].Currencies[dataQuantity[0].Coins-1].Name, name);
        (*pClients)[i].Currencies[dataQuantity[0].Coins-1].quantity = 0;
    }
}

void AddClient(CoPointer pCoins, ClPointer *pClients, DataQuantity *dataQuantity, bool isAdm, char *name, char *cpf, char *pass){
    if((*pClients) == NULL){
        (*pClients) = (ClPointer)calloc(1, sizeof(Client));
    }
    dataQuantity[0].Clients++;
    (*pClients) = (ClPointer)realloc((*pClients), sizeof((*pClients)[0]) * dataQuantity[0].Clients);
    memset(&(*pClients)[dataQuantity[0].Clients-1], 0, sizeof((*pClients)[0]));
    (*pClients)[dataQuantity[0].Clients-1].IsAdm = isAdm;
    strcpy((*pClients)[dataQuantity[0].Clients-1].Name, name);
    strcpy((*pClients)[dataQuantity[0].Clients-1].Cpf, cpf);
    strcpy((*pClients)[dataQuantity[0].Clients-1].Pass, pass);
    (*pClients)[dataQuantity[0].Clients-1].Currencies = (CuPointer)calloc(dataQuantity[0].Coins ,sizeof(Currency));
    for (int i = 0; i < dataQuantity[0].Coins; i++){
        strcpy((*pClients)[dataQuantity[0].Clients-1].Currencies[i].Name, pCoins[i].Name);
    }
}

bool RemoveCoin(ClPointer pClients, CoPointer *pCoins, int indexToRemove, DataQuantity *dataQuantity){
    CoPointer tempPCoins;
    tempPCoins = (CoPointer)calloc(dataQuantity[0].Coins-1, sizeof(Coin));
    if(tempPCoins == NULL){
        perror("erro ao alocar memoria para \"tempPCoins\" em \"AddCoin\"");
        return false;
    }
    int j = 0;
    for (int i = 0; i < dataQuantity[0].Coins; i++){
        if(indexToRemove != i){
            tempPCoins[j] = (*pCoins)[i];
            j++;
        }
    }
    dataQuantity[0].Coins--;
    CuPointer tempCurrencies;
    tempCurrencies = (CuPointer)calloc(dataQuantity[0].Coins, sizeof(Currency));
    if(tempCurrencies == NULL){
        perror("erro ao alocar memoria para \"tempCurrencies\" em \"AddCoin\"");
        return false;
    }
    for(int i = 0; i < dataQuantity[0].Clients; i++){
        j = 0;
        for (int I = 0; I < dataQuantity[0].Coins+1; I++){
            if(I != indexToRemove){
                tempCurrencies[j] = pClients[i].Currencies[I];
                j++;
            }
        }
        pClients[i].Currencies = (CuPointer)realloc(pClients[i].Currencies, dataQuantity[0].Coins * sizeof(Currency));
        for (int I = 0; I < dataQuantity[0].Coins; I++){
            tempCurrencies[I] = pClients[i].Currencies[I];
        }
    }
    (*pCoins) = (CoPointer)realloc((*pCoins), sizeof(Coin) * dataQuantity[0].Coins);
    for(int i = 0; i < dataQuantity[0].Coins; i++){
        (*pCoins)[i] = tempPCoins[i]; 
    }
    free(tempPCoins);
    free(tempCurrencies);
    return true;
}

bool RemoveClient(ClPointer *pClients, int indexToRemove, DataQuantity *dataQuantity){
    ClPointer tempPClients;
    tempPClients = (ClPointer)calloc(sizeof((*pClients)[0]), dataQuantity[0].Clients-1);
    if(tempPClients == NULL){
        perror("erro ao alocar memoria para \"tempPClients\" em \"RemoveClient\"");
        return false;
    }
    int j = 0;
    for (int i = 0; i < dataQuantity[0].Clients; i++){
        if(indexToRemove != i){
            tempPClients[j] = (*pClients)[i];
            j++;
        }
        else{
            free((*pClients)[i].Currencies);
        }
    }
    dataQuantity[0].Clients--;
    (*pClients) = (ClPointer)realloc((*pClients), sizeof((*pClients)[0]) * dataQuantity[0].Clients);
    for(int i = 0; i < dataQuantity[0].Clients; i++){
        (*pClients)[i] = tempPClients[i];
    }
    free(tempPClients);
    return true;
}

void AddExtract(ClPointer (*pClients), int clientIndex, char transactionType, Coin coin, int quantity){
    //transactionType = 'S' para Sell, 'B' para buy, 'R' para retirada ou saque, 'I' para colocar ou depositar
    time_t t;
    time(&t);
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].TransactionType = transactionType;
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].Coin = coin;
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].Quantity = quantity;
    int num;
    if((*pClients)[clientIndex].CurrentExtractIndex-1 == -1){
        num = (*pClients)[clientIndex].Extract[99].IDNumber;
    }
    else{
        num = (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex-1].IDNumber;
    }
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].IDNumber = num;
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].Date = *localtime(&t);
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].Date.tm_mon += 1;
    (*pClients)[clientIndex].Extract[(*pClients)[clientIndex].CurrentExtractIndex].Date.tm_year += 1900;
    (*pClients)[clientIndex].CurrentExtractIndex++;
    if((*pClients)[clientIndex].CurrentExtractIndex == 100){
        (*pClients)[clientIndex].CurrentExtractIndex = 0;
    }

}
//------------------------------------------------------------------------//



/*Menu functions*/
//------------------------------------------------------------------------//
void limparTerminal() {
    printf("\033[H\033[J");
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void Passar(){

    printf("\nPRESSIONE ENTER PARA VOLTAR!");
    getchar();
    getchar();

}

void MenuInicial(){
    limparTerminal();
    printf("---------------Bem Vindo!---------------\n");
    printf("1. Cadastrar-se\n");
    printf("2. Logar\n");
    printf("Escolha entre logar ou cadastrar-se: ");
}

void MenuAdm(ClPointer pClients, int userIndex){
    limparTerminal();
    printf("Bem vindo %s!!\n\n", pClients[userIndex].Name);
    sleep(2);
    printf("     _-+={Menu}=+-_     \n\n");
    printf("  {1} - Cadastrar Investidor   \n");
    printf(" {2} - Excluir investidor  \n");
    printf("  {3} - Cadastrar criptomoeda   \n");
    printf("    {4} - Excluir criptomoeda   \n");
    printf("{5} - Consultar saldo de investidor   \n");
    printf("{6} - Consultar extrado de investidor   \n");
    printf(" {7} - Atualizar cotação   \n");
    printf("       {8} - Sair   \n\n");
    printf("Selecione sua opção:\n");
}

void MenuInvestidor(ClPointer pClients, int userIndex){
    limparTerminal();
    printf("Bem vindo %s!!\n\n", pClients[userIndex].Name);
    sleep(2);
    printf("     _-+={Menu}=+-_     \n\n");
    printf("  {1} - Consultar saldo   \n");
    printf(" {2} - Consultar extrato  \n");
    printf("  {3} - Depositar reais   \n");
    printf("    {4} - Sacar reais   \n");
    printf("{5} - Comprar criptomoedas   \n");
    printf("{6} - Vender criptomoedas   \n");
    printf(" {7} - Atualizar cotação   \n");
    printf("       {8} - Sair   \n\n");
    printf("Selecione sua opção:\n");
}

bool Login(ClPointer pClients, int clientsQuantity,int *indexClient) {
    //Declaração de variáveis
    char cpfEntrada[12];
    char passEntrada[7];
    int indexPlayerLoop;
    bool userAchado = false;
    char resposta;
    bool tudoCerto;

    //Loop para login
    while (true) {

        //Exposição do menu e Solicitação de dados
        //loop para garantir entradas corretas
        while (true)
        {
            limparTerminal();
            printf("\n-------------------Login-------------------\n");
            printf("\nInsira seu CPF: ");
            scanf("%11s", cpfEntrada); 
            limparBufferEntrada();
            if(strlen(cpfEntrada) != 11){
                printf("Quantidade de caracteres inválida! O CPF deve ter 11 caracteres\n");
                Passar();
            }
            else{
                printf("\nInsira sua Senha   : ");
                scanf("%6s", passEntrada);
                limparBufferEntrada();
                if (strlen(passEntrada) != 6)
                {
                    printf("Quantidade de caracteres inválida! A senha deve ter 6 caracteres\n");
                    Passar();
                }
                else{
                    for (int i = 0; passEntrada[i] != '\0'; i++) {
                        if (!isdigit(passEntrada[i])) {
                            printf("A senha é composta somente de números!\n");
                            tudoCerto = false;
                            Passar();
                            break;
                        }
                        else{
                            tudoCerto = true;
                        }
                    
                    }
                }
                if(tudoCerto == true){
                    break;
                }
            }
        }

        //Loop para encontrar o Usuario
        for (indexPlayerLoop = 0; indexPlayerLoop < clientsQuantity; indexPlayerLoop++) {
            //Verifica se o CPF e senha são os mesmos para aquele index
            if (strcmp(pClients[indexPlayerLoop].Cpf, cpfEntrada) == 0 &&
                strcmp(pClients[indexPlayerLoop].Pass, passEntrada) == 0) {
                //caso a condição seja verdadeira, ele faz userAchado = false
                userAchado = true;
                break;
            }
        }

        //Verifica se o usuario foi encontrado
        if (userAchado) {
            //Salva o index do usuario 
            *indexClient = indexPlayerLoop;
            printf("Bem-vindo, %s! Você está logado!\n", pClients[indexPlayerLoop].Name);
            Passar();
            limparTerminal();
            return true;
        } 
        //Caso não tenha sido encontrado
        else {
            //loop para verificar se usuario quer continuar tentando
            while(true){
                printf("Usuário ou senha incorretos. Deseja continuar(1 para sim, 2 para não)?\n");
                scanf(" %1c", &resposta);
                limparBufferEntrada();
                //se sim ele só quebra o loop
                if(resposta == '1'){
                    break;
                }
                //se não ele retora False
                else if(resposta == '2'){
                    return false;
                }
                //Senão(verdadeiro) ele dá que a entrada foi diferente de 1 ou 2
                else{
                    printf("Entrada inválida, tente novamente\n");
                    Passar();
                }

                limparTerminal();
            }
        }
    }
}
//------------------------------------------------------------------------//



/*Base functions*/
//------------------------------------------------------------------------//

void AtualizarCotacoes(CoPointer *pCriptos, int criptosQuantity){
    srand(time(NULL));
    limparTerminal();
    
    char respostaUser[20];
    
    
    while (true){
        printf("Valor das Atual das criptos:\n");
        for (int i = 0; i < criptosQuantity; i++){
            printf("| %20s: | %.2lf | |\n",(*pCriptos)[i].Name, (*pCriptos)[i].Value);
        }
        printf("\nDeseja atualizar? (s/n) \n");
        scanf("%s", respostaUser);
        if (respostaUser[0] == 's'){
            printf("Processando novos valores...\n");
            sleep(4);

            double var;
            for(int i = 0; i < criptosQuantity; i++){
                var = (rand() % 30 - 15)/(double)100;
                (*pCriptos)[i].Value += (*pCriptos)[i].Value * (1 + var);
            }

            for (int i = 0; i < criptosQuantity; i++){
            printf("| %20s: | %.2lf | |\n",(*pCriptos)[i].Name, (*pCriptos)[i].Value);
            }
            getchar();
            return;
        }
        else if (respostaUser[0] == 'n'){
            getchar();
            return;
        }
        else{
            printf("Resposta inválida! Tente novamente.\n");
        }
    }
    limparTerminal();
}

void VenderCriptomoedas(ClPointer *pClients, int userIndex, CoPointer pCriptos, DataQuantity dataQuantities){
    int coinIndex;
    double quantity;
    limparTerminal();

    printf("Qual criptomoeda deseja vender?(responda usando o indice)\n");
    for(int i = 0; i < dataQuantities.Coins; i++){
        printf("|%s(%d): |%.2lf| |\nSellTax: %lf\n",pCriptos[i].Name, i, pCriptos[i].Value, pCriptos[i].SellTax);
    }
    scanf(" %d", &coinIndex);
    if(coinIndex >= dataQuantities.Coins || coinIndex < 0){
        printf("Indice inválido\n");
        getchar();
        return;
    }
    printf("Você tem %.2lf %s(s)\n", (*pClients)[userIndex].Currencies[coinIndex].quantity, (*pClients)[userIndex].Currencies[coinIndex].Name);
    printf("Quantas quer vender?\n");
    scanf(" %lf", &quantity);
    if(quantity < 0){
        printf("Valores negativos são inválidos\n");
        getchar();
        return;
    }
    if(quantity <= (*pClients)[userIndex].Currencies[coinIndex].quantity){
        (*pClients)[userIndex].Currencies[coinIndex].quantity -= quantity;
        (*pClients)[userIndex].Currencies[0].quantity += quantity * pCriptos[coinIndex].Value * (1 - pCriptos[coinIndex].SellTax);
        AddExtract(pClients, userIndex, 'S', pCriptos[coinIndex], quantity);
    }
    else{
        printf("Não tem moedas o suficiente");
    }
}

void ComprarCriptomoedas(ClPointer *pClients, int userIndex, CoPointer pCriptos, DataQuantity dataQuantities){

    char respostaUser[20];
    int coinIndex, senhaResposta;
    double quantity;

    senhaResposta = PedirSenha((*pClients)[userIndex].Pass);
    if(senhaResposta == 0){
        printf("Senha invalida\n");
        getchar();
        return;
    }
    else if(senhaResposta == -1){
        printf("Ação cancelada\n");
        getchar();
        return;
    }
    printf("Qual criptomoeda deseja Comprar?(responda usando o indice)\n");
    for(int i = 0; i < dataQuantities.Coins; i++){
        printf("|%s(%d): |%.2lf| |\nBuyTax: %lf\n",pCriptos[i].Name, i, pCriptos[i].Value, pCriptos[i].BuyTax);
    }
    scanf(" %d", &coinIndex);
    if(coinIndex >= dataQuantities.Coins || coinIndex < 0){
        printf("Indice inválido\n");
        getchar();
        return;
    }
    printf("Você tem %.2lf %s(s)\n", (*pClients)[userIndex].Currencies[0].quantity, (*pClients)[userIndex].Currencies[0].Name);
    printf("Quantas quer Comprar?\n");
    scanf(" %lf", &quantity);
    if(quantity < 0){
        printf("Valores negativos são inválidos\n");
        getchar();
        return;
    }
    if((*pClients)[userIndex].Currencies[0].quantity - (quantity * pCriptos[coinIndex].Value) < 0){
        printf("Quantia insuficiente de %s(s)", (*pClients)[userIndex].Currencies[0].Name);
        getchar();
        return;
    }
    (*pClients)[userIndex].Currencies[0].quantity -= (quantity * pCriptos[coinIndex].Value);
    (*pClients)[userIndex].Currencies[coinIndex].quantity += quantity;
    printf("Compra realizada com sucesso\n");
    AddExtract(pClients,userIndex,'B',pCriptos[coinIndex],quantity);
    getchar();
}

void SacarReais(ClPointer *pClients, int userIndex, CoPointer pCriptos){
    limparTerminal();
    double quantity;
    int senhaResposta = PedirSenha((*pClients)[userIndex].Pass);
    if(senhaResposta == 0){
        printf("Senha invalida\n");
        getchar();
        return;
    }
    else if(senhaResposta == -1){
        printf("Ação cancelada\n");
        getchar();
        return;
    }
    printf("Você tem %.2lf %s(s)\n", (*pClients)[userIndex].Currencies[0].quantity, (*pClients)[userIndex].Currencies[0].Name);
    printf("Quantas quer sacar?\n");
    scanf(" %lf", &quantity);
    if(quantity < 0){
        printf("Valores negativos são inválidos\n");
        getchar();
        return;
    }
    else if(quantity > (*pClients)[userIndex].Currencies[0].quantity){
        printf("Quantia insuficiente para o saque\n");
        getchar();
        return;
    }
    (*pClients)[userIndex].Currencies[0].quantity -= quantity;
    AddExtract(pClients,userIndex,'R', pCriptos[0], quantity);
    printf("Saque realizado com sucesso\n");
    printf("Você tem %.2lf %s(s)\n", (*pClients)[userIndex].Currencies[0].quantity, (*pClients)[userIndex].Currencies[0].Name);
    getchar();
}

void DepositarReais(ClPointer *pClients, int userIndex, CoPointer pCriptos){

    limparTerminal();

    double deposito;
    printf("Quantos reais gostaria de depositar?: ");
    scanf(" %lf", &deposito);
    
    if(deposito <= 0) { 
        printf("[ERRO]! Valor inválido.\n O saque deve ser maior que zero.\n");
        getchar(); 
        return;
    }


    (*pClients)[userIndex].Currencies[0].quantity += deposito;
    AddExtract(pClients,userIndex,'I',pCriptos[0], deposito);
    printf("Você tem %.2lf %s(s)\n", (*pClients)[userIndex].Currencies[0].quantity, (*pClients)[userIndex].Currencies[0].Name);

    getchar();
}

void ConsultarSaldo(ClPointer pClients, int userIndex, DataQuantity dataQuantities){

    limparTerminal();

    printf("===== Consulta de Saldo =====\n\n");
    printf("Nome: %s\n", pClients[userIndex].Name);
    for(int i = 0; i < dataQuantities.Coins; i++){
        printf("|%s(%d): |%.2lf| |\n", pClients[userIndex].Currencies[i].Name, i, pClients[userIndex].Currencies[i].quantity);
    }
    getchar();

}

int PedirSenha(char userSenhaCerta[7]){
    //retorna 1 caso o usuario tenha acertado a senha, 0 caso tenha errado e 0 caso o usuario deseja encerrar a tentativa;
    char respostaUser[20];
    while(true){
        printf("Digite sua senha:\n");
        scanf(" %s", respostaUser);
        if(strlen(respostaUser) > 6){
            printf("Senha digitada inválida, quantidade de caracteres excedida:\n");
            while (true)
            {
                printf("deseja continuar?\'s/n\'\n");
                scanf(" %s", respostaUser);
                if(respostaUser[0] == 'n') break;
                else if (respostaUser[0] == 's') break;
                printf("resposta não reconhecida\n");
            }
            if(respostaUser[0] == 'n'){
                return -1;
            }
            continue;
        }
        else if(strlen(respostaUser) < 6){
            printf("Senha digitada inválida, quantidade mínima de caracteres não preenchida:\n");
            while (true)
            {
                printf("deseja continuar?\'s/n\'\n");
                scanf(" %s", respostaUser);
                if(respostaUser[0] == 'n') break;
                else if (respostaUser[0] == 's') break;
                printf("resposta não reconhecida\n");
            }
            if(respostaUser[0] == 'n'){
                return -1;
            }
            continue;
        }
        int i;
        for(i = 0; i < 6; i++){
            if(respostaUser[i] != userSenhaCerta[i]) return 0;
        }
        return 1;
    }
}

void ConsultarExtratoInvestidor(ClPointer pClients, DataQuantity dataQuantities) {
    char cpfInvestidor[12];
    bool clienteEncontrado = false;

    printf("Digite o CPF do investidor (somente números): ");
    scanf("%11s", cpfInvestidor);

    for (int i = 0; i < dataQuantities.Clients; i++) {
        if (strcmp(pClients[i].Cpf, cpfInvestidor) == 0) {
            clienteEncontrado = true;
            printf("\n=== Extrato do Investidor ===\n");
            printExtrato(pClients, dataQuantities, i);
            break;
        }
    }

    if (!clienteEncontrado) {
        printf("\nInvestidor não encontrado.\n");
    }
}

void ConsultarSaldoInvestidor(ClPointer pClients, DataQuantity dataQuantities) {
    char cpfInvestidor[12];
    bool clienteEncontrado = false;

    printf("Digite o CPF do investidor (somente números): ");
    scanf("%11s", cpfInvestidor);

    for (int i = 0; i < dataQuantities.Clients; i++) {
        if (strcmp(pClients[i].Cpf, cpfInvestidor) == 0) {
            clienteEncontrado = true;
            printf("\n=== Extrato do Investidor ===\n");
            ConsultarSaldo(pClients, i, dataQuantities);
            break;
        }
    }

    if (!clienteEncontrado) {
        printf("\nInvestidor não encontrado.\n");
    }
}

//------------------------------------------------------------------------//