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
void limparBufferEntrada();
void Passar();
void MenuInicial();
void MenuAdm(ClPointer pClients, int userIndex);
void MenuInvestidor(ClPointer pClients, int userIndex);
bool Login(ClPointer pClients, int clientsQuantity,int *indexClient);
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
    char respostaUserPP;
    int *indexClient;

    while(true){

        limparTerminal();
        //Menu principal
        MenuInicial();
        //Pergunta se o usuario que se cadastrar ou logar
        scanf(" %c", &respostaUserPP);

        //Cadastro
        if(respostaUserPP == '1'){
            limparTerminal();
            printf(":)");
        }
        //Login
        else if(respostaUserPP == '2'){
            limparTerminal();
                //CHAMADA TEMPORARIA DE LOGIN, NECESSARIO CRIAR O DATAQUANTITY
            Login(pClients, 10, indexClient);
        }
        //Caso não seja 1 ou 2
        else{
            limparTerminal();
            printf("Input inválido, tente novamente...");
            Passar();
        }
    //------------------------------------------------------------------------//
    free(pCoins);
    FreePClients(pClients, dataQuantity);
    }
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
                printf("||Tipo de Transação: %s || Valor: %lf || Tipo de Transação: %s || Nome da Moeda: %s || Valor da Moeda: %lf || Taxa de Venda: %s "
                "|| Taxa de Compra: %lf || ID do Extrato: %d || Quantidade Movida: %lf ||"
                "Horário: %d:%d:%d ||\n", 
                pClients[index].Extract[IndexExtract].TransactionType, pClients[index].Extract[IndexExtract].Coin.Name, 
                pClients[index].Extract[IndexExtract].Coin.Value, pClients[index].Extract[IndexExtract].Coin.SellTax,
                pClients[index].Extract[IndexExtract].Coin.BuyTax,pClients[index].Extract[IndexExtract].IDNumber,
                pClients[index].Extract[IndexExtract].Quantity, pClients[index].Extract[IndexExtract].Date.tm_hour,
                pClients[index].Extract[IndexExtract].Date.tm_min, pClients[index].Extract[IndexExtract].Date.tm_sec);
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
        (*pClients) = (ClPointer)Calloc(1, sizeof(Client));
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
    tempCurrencies = (CuPointer)Calloc(dataQuantity[0].Coins, sizeof(Currency));
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
    printf("Bem vindo %s!!\n\n", pClients[userIndex].Nome);
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
    printf("Bem vindo %s!!\n\n", pClients[userIndex].Nome);
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
            scanf("%11s", &cpfEntrada); 
            limparBufferEntrada();
            if(strlen(cpfEntrada) != 11){
                printf("Quantidade de caracteres inválida! O CPF deve ter 11 caracteres\n");
                Passar();
            }
            else{
                printf("\nInsira sua Senha   : ");
                scanf("%6s", &passEntrada);
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
                strcmp(pClients[indexPlayerLoop].Senha, passEntrada) == 0) {
                //caso a condição seja verdadeira, ele faz userAchado = false
                userAchado = true;
                break;
            }
        }

        //Verifica se o usuario foi encontrado
        if (userAchado) {
            //Salva o index do usuario 
            *indexClient = indexPlayerLoop;
            printf("Bem-vindo, %s! Você está logado!\n", pClients[indexPlayerLoop].Nome);
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
        printf("|%s(%d): |%.2lf| |\n",pCriptos[i].Name, i, pCriptos[i].Value);
    }
    scanf(" %d", &coinIndex);
    printf("Você tem %.2lf %s(s)\n", (*pClients)[userIndex].Currencies[coinIndex].quantity, (*pClients)[userIndex].Currencies[coinIndex].Name);
    printf("Quantas quer vender?\n");
    scanf(" %lf", &quantity);
    if(quantity <= (*pClients)[userIndex].Currencies[coinIndex].quantity){
        (*pClients)[userIndex].Currencies[coinIndex].quantity -= quantity;
        (*pClients)[userIndex].Currencies[0].quantity += quantity * pCriptos[coinIndex].Value * (1 - pCriptos[coinIndex].SellTax);
        AddExtract(pClients, userIndex, 'S', pCriptos[coinIndex], quantity);
    }
    else{
        printf("Não tem moedas o suficiente");
    }
}

void ComprarCriptomoedas(CPointer pClients, int userIndex, MPointer bitcoin, MPointer ethereum, MPointer ripple, EPointer ppExtrato, char userSenha[7]){

    char respostaUser[20];
    double valorCompra, valorFinal;
    double taxa, valorCripto;
    int opcaoMoeda;
    int pedirSenhaSaque = PedirSenha(userSenha);

    limparTerminal();


    if(pedirSenhaSaque == 1){ 
    printf("Qual criptomoeda deseja comprar?!\n");
    printf("1 - Bitcoin\n");
    printf("2 - Ethereum\n");
    printf("3 - Ripple\n");
    scanf("%d", &opcaoMoeda);

    limparTerminal();

    printf("Digite o valor que você deseja investir (em reais):\n");
    scanf("%lf", &valorCompra);

    limparTerminal();

    if(valorCompra <= 0){
        printf("O valor deve ser maior que zero!\n");
        getchar();
        return;
    }

    if (valorCompra > pClients[userIndex].Reais) {
        printf("Saldo insuficiente para realizar a operação!.\n");
        getchar();
        return;
    }

    switch(opcaoMoeda) {
    case 1: 
        taxa = valorCompra * bitcoin->TaxaCompra;  
        valorCripto = valorCompra / bitcoin->Valor;  
        printf("Com o valor investido você comprará %.10lf Bitcoins, com uma taxa de R$%.2lf\nO total da transação será de R$%.2lf.\nConfirmar? (s/n)\n", valorCripto, taxa, (valorCompra+taxa));
        break;
    case 2: 
        taxa = valorCompra * ethereum->TaxaCompra;  
        valorCripto = valorCompra / ethereum->Valor;  
        printf("Com o valor investido você comprará %.10lf Ethereums, com uma taxa de R$%.2lf\nO total da transação será de R$%.2lf.\nConfirmar? (s/n)\n", valorCripto, taxa, (valorCompra+taxa));
        break;
    case 3: 
        taxa = valorCompra * ripple->TaxaCompra;  
        valorCripto = valorCompra / ripple->Valor;  
        printf("Com o valor investido você comprará %.10lf Ripples, com uma taxa de R$%.2lf\nO total da transação será de R$%.2lf.\nConfirmar? (s/n)\n", valorCripto, taxa, (valorCompra+taxa));
        break;
    default:
        printf("Opção inválida.\n");
        return;
    }

    
    scanf(" %s", respostaUser);
    if (respostaUser[0] == 's') {
        pClients[userIndex].Reais -= (valorCompra + taxa); 


        switch(opcaoMoeda) {
            case 1:
                pClients[userIndex].Bitcoin += valorCripto;
                break;
            case 2:
                pClients[userIndex].Ethereum += valorCripto;
                break;
            case 3:
                pClients[userIndex].Ripple += valorCripto;
                break;
        }

        switch(opcaoMoeda) {
        case 1: 
            AdicionarExtrato(ppExtrato, bitcoin, "Bitcoin", pClients, userIndex, 'C', valorCripto); 
            break;
        case 2: 
            AdicionarExtrato(ppExtrato, ethereum, "Ethereum", pClients, userIndex, 'C', valorCripto);
            break;
        case 3: 
            AdicionarExtrato(ppExtrato, ripple, "Ripple", pClients, userIndex, 'C', valorCripto); 
            break;
        default:
            printf("[ERRO] Extrato Inválido.\n");
            getchar();
            return;
        }

        limparTerminal();
        printf("Compra realizada com sucesso!\n");
        getchar();
    } else {
        limparTerminal();
        printf("Compra cancelada!\n");
        getchar();
    }
    }
    else{
        printf("Senha incorreta! Operação cancelada.\n");
        getchar();

    }


}

void SacarReais(CPointer pClients, int userIndex, char userSenha[7], EPointer ppExtrato){


    limparTerminal();


    int pedirSenhaSaque = PedirSenha(userSenha);
    double saque;
    
    if(pedirSenhaSaque == 1){  
        printf("Quantos reais gostaria de sacar?: ");
        scanf(" %lf", &saque);

        if(saque <= 0) { 
            printf("[ERRO]! Valor inválido.\n O saque deve ser maior que zero.\n");
            getchar();
            return;
        }

        if(pClients[userIndex].Reais < saque){  
            printf("Em sua conta não a saldo suficiente para realizar essa operação!\n");

            getchar();
        }
        else{
            pClients[userIndex].Reais = pClients[userIndex].Reais - saque;  

            printf("R$ %.2lf sacados da sua conta!\n", saque);
            printf("Saldo Atual: R$ %.2lf\n", pClients[userIndex].Reais);
            AdicionarExtrato(ppExtrato, NULL, "Reais", pClients, userIndex, 'S', saque);

            getchar();
        }
    }
    else{
        printf("Senha incorreta! Operação cancelada.\n");
        getchar();

    }
}

void DepositarReais(CPointer pClients, int userIndex, EPointer ppExtrato){

    limparTerminal();

    double deposito;
    printf("Quantos reais gostaria de depositar?: ");
    scanf(" %lf", &deposito);
    
    if(deposito <= 0) { 
        printf("[ERRO]! Valor inválido.\n O saque deve ser maior que zero.\n");
        getchar(); 
        return;
    }


    pClients[userIndex].Reais += deposito;
    
    AdicionarExtrato(ppExtrato, NULL, "Reais", pClients, userIndex, 'D', deposito);

    printf("Você depositou R$ %.2lf\nTotal na conta: R$ %.2lf\n",deposito, pClients[userIndex].Reais); 

    getchar();
}

void ConsultarSaldo(CPointer pClients, int userIndex){

    limparTerminal();

    printf("===== Consulta de Saldo =====\n\n");
    printf("Nome: %s\n", pClients[userIndex].Nome);
    printf("Saldo em Reais: R$ %.2lf\n", pClients[userIndex].Reais);
    printf("Saldo em Bitcoin: %.8lf BTC\n", pClients[userIndex].Bitcoin);
    printf("Saldo em Ethereum: %.8lf ETH\n", pClients[userIndex].Ethereum);
    printf("Saldo em Ripple: %.8lf XRP\n", pClients[userIndex].Ripple);

    getchar();



}

int PedirSenha(char userSenhaCerta[7]){
    //retorna 1 caso o usuario tenha acertado a senha, 0 caso tenha errado e -1 caso o usuario deseja encerrar a tentativa;
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

//------------------------------------------------------------------------//