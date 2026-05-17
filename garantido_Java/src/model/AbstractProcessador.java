package model;

public interface AbstractProcessador {
	boolean isLivre();
	Processo retira();
	void recebe(Processo processo);
	void processa();
	boolean acabou();
}
