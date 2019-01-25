package controller;

import java.util.Iterator;

import model.FilaEntrada;
import model.FilaProcessos;
import model.Memoria;
import model.Processador;
import model.Processo;
import model.Processo.Estado;
import model.Processo.Prioridade;
import view.TelaJFrame;

public class Escalonador implements AbstractEscalonador {
	private FilaEntrada filaEntrada;
	private FilaProcessos filaProcessos;
	private FilaProcessos filaProcessosSuspensos;
	private Processador CPU;
	private static Escalonador instancia;
	private boolean start = false;

	public static Escalonador getInstancia() {
		if (instancia == null)
			return instancia = new Escalonador();
		else
			return instancia;
	}

	/**
	 * Inicializa o escalonador
	 * 
	 * @param fila
	 *            , recebe fila de entrada com os processos contida no mesmo.
	 */
	private Escalonador() {
		this.filaProcessos = new FilaProcessos();
		this.filaProcessosSuspensos = new FilaProcessos();
		this.CPU = Processador.getInstancia();
	}

	/**
	 * Verifica se há espaço disponível na memória, caso sim, aloca espaço na
	 * mesma para o processo e o adiciona na filaProcessos.
	 */
	public void inicializarFilaProcessos(FilaEntrada filaEntrada) {
		Memoria memoria = CPU.getMemoria();

		while ((!filaEntrada.vazia())
				&& memoria.espacoLivre(filaEntrada.head().getMb())) {
			Processo processo = filaEntrada.remover();
			processo.setEstado(Estado.PRONTO);
			memoria.armazena(processo);
			filaProcessos.add(processo);
			TelaJFrame.atualizarLinha(processo);
		}
		desenharCabecalho();
		definirQuantum();
		this.start = true;
	}

	private void desenharCabecalho() {
		TelaJFrame.setTextMemoria(
				String.valueOf(CPU.getMemoria().getEspacoUtilizado() * 4),
				String.valueOf(CPU.getMemoria().TAMANHO_MEMORIA * 4));
		TelaJFrame.setTextQtdProcessos(String.valueOf(filaProcessos.tamanho()));
	}

	public void finalizar() {
		Processo processo = CPU.retira();
		processo.setEstado(Estado.FINALIZADO);
		CPU.getMemoria().libera(processo);
		
		desenharCabecalho();
		TelaJFrame.atualizarLinha(processo);
	}

	/**
	 * Suspende o processo em execução no processador.
	 */
	@Override
	public void suspender() {
		Processo processo = CPU.retira();

		if (processo != null) {
			processo.setEstado(Estado.SUSPENSO);
			TelaJFrame.atualizarLinha(processo);
			filaProcessosSuspensos.add(processo);
		}
	}

	public void iniciar() {
		if (!filaProcessosSuspensos.vazia()) {
			Processo processo = filaProcessosSuspensos.remover();
			processo.setEstado(Estado.PRONTO);
			filaProcessos.add(processo);
			TelaJFrame.atualizarLinha(processo);
		}
	}

	/**
	 * Verifica qual o próximo processo a ser executado.
	 * 
	 * @return processo, retorna o próximo processo a ser executado.
	 */
	@Override
	public Processo getProximoProcesso() {
		Processo processo;

		if (!filaProcessos.vazia()) {
			processo = filaProcessos.remover();
			return processo;
		}

		return null;
	}

	public double calcularQuantumParcial() {
		double tempoTotal = 0;

		Iterator<Processo> i = filaProcessos.getIterator();
		while (i.hasNext()) {
			Processo processo = i.next();
			tempoTotal += ((Processo) processo).getTempoProcessamento();
		}

		return tempoTotal / filaProcessos.tamanho();
	}

	/**
	 * Calcula a razao entre o tempo de vida do processo e o quantum do
	 * processador cada processo deveria ter, se menor que 0, igual a 1, maior
	 * que 1, seta a prioridade do processo para BAIXA, MEDIA, ALTA
	 * respectivamente.
	 */
	private void alteraPrioridade() {
		if (filaProcessos.tamanho() == 0)
			return;

		double quantumParcial = calcularQuantumParcial();

		Iterator<Processo> i = filaProcessos.getIterator();

		while (i.hasNext()) {
			Processo processo = i.next();

			double razaoQuantum = processo.getTempoProcessamento()
					/ quantumParcial;
			processo.setRazaoQuantum(razaoQuantum);

			if (processo.getRazaoQuantum() > 1) {
				processo.setPrioridade(Prioridade.BAIXA);
			} else if (processo.getRazaoQuantum() == 1) {
				processo.setPrioridade(Prioridade.MEDIA);
			} else {
				processo.setPrioridade(Prioridade.ALTA);
			}
			TelaJFrame.atualizarLinha(processo);
		}
	}

	/**
	 * calcula e retorna o quantum cada processo deve ter do processador. (1/n
	 * processos).
	 * 
	 * @return quatum
	 */
	private int definirQuantum() {
		int quantum = filaProcessos.tamanho() * 2 / filaProcessos.tamanho();
		CPU.setQuantum(quantum);
		return quantum;

	}

	@Override
	public void escalona() {
		while (true) {
			while (start && !filaProcessos.vazia()) {

				Processo processo = getProximoProcesso();
				CPU.recebe(processo);
				processo.setEstado(Estado.EXECUTANDO);

				CPU.processa();

				if (CPU.acabou()) {
					processo = CPU.retira();

					if (processo.getSurto() == 0) {
						processo.setEstado(Estado.FINALIZADO);
						CPU.getMemoria().libera(processo);
						desenharCabecalho();
					} else {
						processo.setEstado(Estado.PRONTO);
						filaProcessos.add(processo);
					}
					TelaJFrame.atualizarLinha(processo);
					alteraPrioridade();

				}
			}
		}
	}

	public Processador getCPU() {
		return CPU;
	}

	public FilaProcessos getFilaProcessos() {
		return filaProcessos;
	}

	public FilaEntrada getFilaEntrada() {
		return filaEntrada;
	}

	public FilaProcessos getFilaProcessosSuspensos() {
		return filaProcessosSuspensos;
	}
}
