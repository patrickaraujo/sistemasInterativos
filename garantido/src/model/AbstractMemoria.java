package model;

public interface AbstractMemoria {
	void libera(Processo processo);
	void armazena(Processo processo);
	int getPosicao(int numeroBlocos);
	void desfragmenta();
	boolean espacoLivre(int tamanho);
}
