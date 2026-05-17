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

	public static synchronized Escalonador getInstancia() {
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
		this.filaEntrada = new FilaEntrada();
		this.filaProcessos = new FilaProcessos();
		this.filaProcessosSuspensos = new FilaProcessos();
		this.CPU = Processador.getInstancia();
	}

	/**
	 * Verifica se há espaço disponível na memória, caso sim, aloca espaço na
	 * mesma para o processo e o adiciona na filaProcessos.
	 */
	public void inicializarFilaProcessos(FilaEntrada filaEntrada) {
		this.filaEntrada = filaEntrada;
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
		if (processo == null) {
			return;
		}
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
		if (filaProcessos.tamanho() == 0) {
			return 0;
		}

		double tempoTotal = 0;

		Iterator<Processo> i = filaProcessos.getIterator();
		while (i.hasNext()) {
			Processo processo = i.next();
			tempoTotal += processo.getTempoProcessamento();
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
		if (quantumParcial == 0) {
			// Nenhum processo executou ainda; todos ficam com prioridade média
			return;
		}

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
	 * Define o quantum baseado no número de processos prontos.
	 * Em escalonamento garantido, cada processo deve receber 1/N do tempo de CPU.
	 * Aqui usamos quantum proporcional: quanto mais processos, menor o quantum
	 * por rodada, com um mínimo de 2 e um máximo de 10.
	 *
	 * @return quantum calculado
	 */
	private int definirQuantum() {
		int qtd = filaProcessos.tamanho();
		if (qtd == 0) {
			CPU.setQuantum(2);
			return 2;
		}
		int quantum = Math.max(2, Math.min(10, 20 / qtd));
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

			// Aguarda um pouco antes de checar de novo, para não consumir 100% de CPU
			// quando a fila está vazia. O usuário pode adicionar mais processos pela UI.
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
				return;
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
