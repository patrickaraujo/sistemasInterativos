package model;


public class Processo extends Thread implements Comparable<Processo>{
	private int id;
	private String nome;
	private int mb;
	private int tempoProcessamento;
	private int quantum;
	private int surto;
	private double razaoQuantum;
	private Estado estado;
	private Prioridade prioridade;

	public enum Prioridade {
		ALTA , MEDIA , BAIXA ;
	}

	public enum Estado {
		NOVO, PRONTO, SUSPENSO, EXECUTANDO, FINALIZADO;
	}

	public Processo(int id, String nome, int mb, Prioridade prioridade,
			Estado estado, int surto) {
		this.id = id;
		this.nome = nome;
		this.mb = mb;
		this.prioridade = prioridade;
		this.estado = estado;
		this.quantum = 0;
		this.razaoQuantum = 0;
		this.tempoProcessamento = 0;
		this.surto = surto;
	}
	
	public Processo() {
		this.id = 0;
	}

	public String toString() {
		String descricao = "ID: " + id + " | Nome: " + nome + " | Tamanho: "
				+ mb + " | Prioridadade: " + prioridade + " | Estado: "
				+ estado + " | Quantum: " + quantum + " | Quantum Parcial: "
				+ razaoQuantum + " | Tempo Processamento: "
				+ tempoProcessamento + " | ";

		return descricao;
	}

	public void decrementaQuantum() {
		this.quantum--;
		this.tempoProcessamento++;
	}

	public int getQuantum() {
		return this.quantum;
	}

	public int getID() {
		return id;
	}

	public int getMb() {
		return mb;
	}

	public void setEstado(Estado estado) {
		this.estado = estado;
	}
	
	/*public void setTempoProcessamento(int tempoProcessamento) {
		this.tempoProcessamento += tempoProcessamento;
	}*/

	public void setQuantum(int quantum) {
		this.quantum = quantum;
	}
	
	public void incrementaQuantum() {
		System.out.println(++quantum);
		--surto;
		++tempoProcessamento;
	}
	@Override
	public int compareTo(Processo processo) {
		return this.prioridade.compareTo(processo.prioridade);
	}

	public void setPrioridade(Prioridade prioridade) {
		this.prioridade = prioridade;
	}

	public int getTempoProcessamento() {
		return tempoProcessamento;
	}

	public void setRazaoQuantum(double razaoQuantum) {
		this.razaoQuantum = razaoQuantum;
	}

	public double getRazaoQuantum() {
		return razaoQuantum;
	}

	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Processo) {
			return this.id == ((Processo)obj).getID();
		}
		
		return false;
	}

	@Override
	public int hashCode() {
		return id;
	}

	public String getNome() {
		return nome;
	}

	public void setNome(String nome) {
		this.nome = nome;
	}

	public Estado getEstado() {
		return estado;
	}

	public Prioridade getPrioridade() {
		return prioridade;
	}

	public void setID(int id) {
		this.id = id;
		
	}

	public int getSurto() {
		return surto;
	}

	public void setSurto(int surto) {
		this.surto = surto;
	}
}
