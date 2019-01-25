package model;


public class Memoria implements AbstractMemoria{
	private BlocoMemoria[] bloco;
	public  final int TAMANHO_MEMORIA = 32;
	private int espacoLivre = 32;	
	
	public Memoria() {
		bloco = new BlocoMemoria[32];
	}
	
	public String toString() {
		return "Memória ocupada: " + getEspacoUtilizado() + " | " + "Memória livre: " + espacoLivre;  
	}
	
	public void imprime() {
		for(BlocoMemoria b : bloco) {
			if (b != null)
				System.out.println(b.toString());
		}
	}
	
	@Override
	public void libera(Processo processo) {
		for (int i = 0; i < bloco.length; i++) {
			if (bloco[i] != null) {
				if (bloco[i].getID() == processo.getID()) {
					bloco[i] = null;
					espacoLivre++;
				}
			}
		}
	}

	@Override
	public void armazena(Processo processo) {
		int numeroBlocos = processo.getMb()/4;
		
		if (processo.getMb() % 4 != 0) {
			numeroBlocos++;
		}
		
		int posicao = getPosicao(numeroBlocos);
			for (int i = posicao; i < (posicao + numeroBlocos); i++) {
				bloco[i] = new BlocoMemoria(processo.getID());
				espacoLivre--;
			}
	}

	@Override
	public int getPosicao(int numeroBlocos) {
		int qtd = 0;
		int i = 0;
		
		while ((i < bloco.length) && (qtd < numeroBlocos)) {
			if (bloco[i] == null)
				qtd++;
			else 
				qtd = 0;
			
			i++;
		}
		
		return i - qtd;
	}

	@Override
	public void desfragmenta() {
		
	}

	@Override
	public boolean espacoLivre(int tamanho) {
		int numeroBlocos = tamanho /4;
		int qtd = 0;
		
		if (tamanho % 4 != 0)
			numeroBlocos++;
		
		int i = 0;
		
		while (i < bloco.length) {
			if (bloco[i] == null) 
				qtd++;
			else
				qtd = 0;
					
			if (qtd >= numeroBlocos)
				return true;
			i++;
		}
		
		return false;
	}

	public int getEspacoLivre() {
		return espacoLivre;
	}

	public int getEspacoUtilizado() {
		return TAMANHO_MEMORIA - espacoLivre;
	}
}

