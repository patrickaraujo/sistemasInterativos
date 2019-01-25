package model;

import view.TelaJFrame;

public class Processador implements AbstractProcessador {
	private Processo processoExectuando;
	private Memoria memoria;
	private int tempoVida;
	private int quantum;
	private int tempoVidaParcial;
	
	private static Processador instancia;

	public static Processador getInstancia() {
		if (instancia == null)
			return instancia = new Processador(new Memoria());
		else
			return instancia;
	}

	private Processador(Memoria memoria) {
		processoExectuando = null;
		this.memoria = memoria;
	}

	@Override
	public boolean isLivre() {
		if (processoExectuando == null)
			return true;

		return false;
	}

	@Override
	public Processo retira() {
		Processo processoAux = processoExectuando;
		processoAux.setQuantum(0);
		processoExectuando = null;
		

		return processoAux;
	}

	@Override
	public void recebe(Processo processo) {
		processoExectuando = processo;
		// memoria.armazena(processo);
	}

	@Override
	public void processa() {
		while (!isLivre() && !acabou()) {
			++tempoVida;
			++tempoVidaParcial;
			processoExectuando.incrementaQuantum();
			drawView();
			
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

	private void drawView() {
		TelaJFrame.atualizarLinha(processoExectuando);
		TelaJFrame.efeitoLinha(processoExectuando);
		TelaJFrame.SetTextTempo(String.valueOf(tempoVida));
	}

	@Override
	public boolean acabou() {
		if (processoExectuando != null) {
			if (processoExectuando.getQuantum() == this.quantum
					|| processoExectuando.getSurto() == 0) {
				return true;
			}
		}
		return false;
	}

	public Memoria getMemoria() {
		return this.memoria;
	}

	public Processo getProcessoExecutando() {
		return this.processoExectuando;
	}

	public int getTempoVida() {
		return tempoVida;
	}

	public void setTempoVida(int tempoVida) {
		this.tempoVida = tempoVida;
	}

	public int getQuantum() {
		return quantum;
	}

	public void setQuantum(int quantum) {
		this.quantum = quantum;
	}

	public int getTempoVidaParcial() {
		return tempoVidaParcial;
	}

	public void setTempoVidaParcial(int tempoVidaParcial) {
		this.tempoVidaParcial = tempoVidaParcial;
	}
}
