
#include <iostream>
using namespace std;

#include <fstream>
#include <string>
#include <vector>


#define MAX_ASCII 256


class Node{
	public:
		int caracter;
		int freq;
		
		Node *proximo;
		Node *esquerda;
		Node *direita;
		
		Node(){
			esquerda = direita = NULL;
		}
		
		Node(int freq){
			this->freq = freq;
			esquerda = direita = proximo = NULL;
		}
	
		Node(int caracter, int freq){
			this->caracter = caracter;
			esquerda = direita = proximo = NULL;
			this->freq = freq;
		}
};


Node *node, *primeiro = NULL, *ultimo = NULL;
vector< vector<bool> > mapa(MAX_ASCII);
vector<bool> caminho;
string simbolos;
int count_simbolo, count_bit;
ofstream saida;
vector<bool> texto;
short K; 

void Compactar(char *caminho_entrada, char* caminho_saida);
void Descompactar(char *caminho_entrada, char* caminho_saida);


void Inserir_na_Lista(Node *node);
void Deletar_Arvore(Node *node);
void Atribuir_Codigo(Node *node, vector< bool > codigo);


void Montar_Arvore(Node *node, char simbolo[]);
void Andar_Pela_Arvore(Node *node);


int main(int argc, char* argv[]){
	

	if(argv[1][0] == 'c'){
		Compactar(argv[2], argv[3]);
	}else if(argv[1][0] == 'd'){
		Descompactar(argv[2], argv[3]);
	}
	

	Deletar_Arvore(primeiro);
	return 0;
}


void Deletar_Arvore(Node *node){
	if(NULL != node->direita){
		Deletar_Arvore(node->direita);
		Deletar_Arvore(node->esquerda);
	}
	delete node;
}


void Atribuir_Codigo(Node *node, vector< bool > codigo = vector<bool>()){
	if(!codigo.empty()){
		caminho.push_back(codigo.back());
	}
	if(NULL != node->direita){
		codigo.push_back(false);
		Atribuir_Codigo(node->esquerda, codigo);
		codigo.back() = true;
		Atribuir_Codigo(node->direita, codigo);
	}else{
		mapa[node->caracter] = codigo;
		simbolos += node->caracter;
	}
}


void Inserir_na_Lista(Node *node){
	
	if(NULL != primeiro){
		if(node->freq <= primeiro->freq){
			node->proximo = primeiro;
			primeiro = node;
		}else if(node->freq >= ultimo->freq){
			ultimo->proximo = node;
			ultimo = node;
		}else{
			Node *move = primeiro->proximo, *anterior = primeiro;
			
			while(node->freq > move->freq){
				anterior = move;
				move = move->proximo;
			}
			
			anterior->proximo = node;
			node->proximo = move;
		}
	}else{
		primeiro = ultimo = node;
	}
}


void Compactar(char *caminho_entrada, char* caminho_saida){

	ifstream entrada(caminho_entrada);
	if(!entrada){
		cerr << "Erro: Nao foi possivel abrir o arquivo " << caminho_entrada << ".";
		return;
	}
	
	saida.open("saida.txt");
	if(!saida){
		cerr << "Erro: Nao foi possivel abrir o arquivo " << caminho_saida << ".";
		return;
	}
	
	K = 0;
	int T = 0;
	
	string linha;
	vector< int > frequencia(MAX_ASCII, 0);
	

	getline(entrada, linha);
	do{
		
		for(unsigned int aux = 0; aux < linha.size(); aux++){
			T++;
			(frequencia[linha[aux]])++;
		}

		getline(entrada, linha);
	}while(!(entrada.eof()));
	

	Node *node;
	
	for(unsigned int aux = 0; aux < MAX_ASCII; aux++){
		if(0 != frequencia[aux]){
			K++;
			node = new Node(aux, frequencia[aux]);
			Inserir_na_Lista(node);
		}
	}
	
	
	while(NULL != primeiro->proximo){
		node = new Node(primeiro->freq + primeiro->proximo->freq);
		node->esquerda = primeiro;
		node->direita = primeiro->proximo;
		primeiro = primeiro->proximo->proximo;
		Inserir_na_Lista(node);
	}
	

	Atribuir_Codigo(primeiro);
	
	if(1 == K){
	
		mapa[primeiro->caracter].push_back(false);
		caminho.push_back(false);
	}
	

	entrada.clear();
	entrada.seekg(ios::beg);
	entrada.clear();

	saida << ((char) K) << ((char) (K >> 8));
	
	saida << ((char) T) << ((char) (T >> 8)) << ((char) (T >> 16)) << ((char) (T >> 24)) ;

	saida << simbolos;

	texto = caminho;

	getline(entrada, linha);
	do{
		for(unsigned int aux = 0; aux < linha.size(); aux++){
			texto.insert(texto.end(), mapa[linha[aux]].begin(), mapa[linha[aux]].end());
		}
		getline(entrada, linha);
	}while(!(entrada.eof()));
	

	char t;
	unsigned int aux;

	for(aux = 0; aux <= (texto.size() - 8);){
	
		for(int bi = 0; bi < 8; bi++){
			t = (t << 1) + (texto[aux++] ? 1 : 0);
		}
	
		saida << t;
	}
	

	if(aux != texto.size()){
		saida << (t << (texto.size() - aux));
	}
}


void Descompactar(char *caminho_entrada, char* caminho_saida){

	ifstream entrada(caminho_entrada, ios::binary);
	if(!entrada){
		cerr << "Erro: Nao foi possivel abrir o arquivo " << caminho_entrada << ".";
		return;
	}
	
	saida.open(caminho_saida);
	if(!saida){
		cerr << "Erro: Nao foi possivel abrir o arquivo " << caminho_saida << ".";
		return;
	}
	

	char byte_lido[6];
   entrada.read(byte_lido, 6);

	K = (((short)byte_lido[1]) << 8) + byte_lido[0];

	int T = (((int)byte_lido[5]) << 24) + (((int)byte_lido[4]) << 16) + (((int)byte_lido[3]) << 8) + byte_lido[2];
	

	char *simbolo = new char[K];
	entrada.read(simbolo, K);
	

	char caracter;
	while(entrada.get(caracter)){
		for(int aux = 7; aux >= 0; aux--){
			texto.push_back(((caracter >> aux) & 1) == 1);
		}
	}
	

	count_bit = count_simbolo = 0;
	primeiro = new Node();
	Montar_Arvore(primeiro, simbolo);
	delete[] simbolo;
	

	count_simbolo = 0;

	while(count_simbolo < T){
		Andar_Pela_Arvore(primeiro);
	}
}


void Montar_Arvore(Node *node, char simbolo[]){
	
	if(K == (count_simbolo + 1) || (texto[count_bit++])){
	
		node->caracter = simbolo[count_simbolo++];
	}else{
		
		node->esquerda = new Node();
		Montar_Arvore(node->esquerda, simbolo);
	
		node->direita = new Node();
		Montar_Arvore(node->direita, simbolo);
	}
}


void Andar_Pela_Arvore(Node *node){

	if(NULL == node->direita){
	
		saida << (char)(node->caracter);
		count_simbolo++;
	}else{
	
		if(texto[count_bit++]){
		
			Andar_Pela_Arvore(node->direita);
		}else{
		
			Andar_Pela_Arvore(node->esquerda);
		}
	}
}
