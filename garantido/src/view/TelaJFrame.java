package view;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;

import controller.Escalonador;
import model.FilaEntrada;
import model.ModeloTabelaProcessos;
import model.Processo;

public class TelaJFrame extends JFrame {
	private static final long serialVersionUID = 1L;
	
	private static JTable tabelaProcessos;
	private static ModeloTabelaProcessos modelo;
	private static JLabel lblTempo;
	private static JLabel lblProcessos;
	private static JLabel lblMemoria;
	private FilaEntrada filaEntrada;
	
	public TelaJFrame() {
		super("Escalonamento Garantido - By Pimpolhos");
		
		filaEntrada = new FilaEntrada();
		
		String[] colunas = { "ID", "Nome", "Prioridade", "Estado",
				"Duração", "T. Processamento","Quantum" };
		modelo = new ModeloTabelaProcessos(new ArrayList<String[]>(), colunas);
		tabelaProcessos = new JTable(modelo);
		
		tabelaProcessos.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
		tabelaProcessos.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		tabelaProcessos.getColumnModel().getColumn(0).setPreferredWidth(25);
		tabelaProcessos.getColumnModel().getColumn(1).setPreferredWidth(55);
		tabelaProcessos.getColumnModel().getColumn(2).setPreferredWidth(100);
		tabelaProcessos.getColumnModel().getColumn(3).setPreferredWidth(120);
		tabelaProcessos.getColumnModel().getColumn(5).setPreferredWidth(120);
		
		JScrollPane jScrollPane = new JScrollPane(tabelaProcessos);
		jScrollPane.setViewportView(tabelaProcessos);  

		JButton btnCriar = new JButton("Criar");
		btnCriar.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				try {
					int surto = Integer.parseInt(JOptionPane.showInputDialog("Entre com o surto"));
					Processo processo = filaEntrada.add(surto);
					addLinha(processo);
				} catch (RuntimeException ex) {
					ex.printStackTrace();
				}
			}
		});
		
		JButton btnStart =  new JButton("Escalonar");
		btnStart.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				Escalonador escalonador = Escalonador.getInstancia();
				escalonador.inicializarFilaProcessos(filaEntrada);
			}
		});
		
		JButton btnSuspender = new JButton("Suspender");
		btnSuspender.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				Escalonador escalonador = Escalonador.getInstancia();
				escalonador.suspender();
			}
		});
		
		JButton btnProsseguir = new JButton("Prosseguir");
		btnProsseguir.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				Escalonador escalonador = Escalonador.getInstancia();
				escalonador.iniciar();
			}
		});
		
		JButton btnFinalizar = new JButton("Finalizar");
		btnFinalizar.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				Escalonador escalonador = Escalonador.getInstancia();
				escalonador.finalizar();
			}
		});

		JPanel panelButtons = new JPanel();
		panelButtons.add(btnCriar);
		panelButtons.add(btnStart);
		panelButtons.add(btnSuspender);
		panelButtons.add(btnProsseguir);
		panelButtons.add(btnFinalizar);

		lblTempo = new JLabel("Tempo:");
		lblProcessos = new JLabel("Processos:");
		lblMemoria = new JLabel("Memória:");

		JPanel pnlCabecalho = new JPanel();
		pnlCabecalho.add(lblTempo);
		pnlCabecalho.add(lblProcessos);
		pnlCabecalho.add(lblMemoria);

		add(pnlCabecalho, BorderLayout.PAGE_START);
		add(jScrollPane, BorderLayout.CENTER);
		add(panelButtons, BorderLayout.PAGE_END);
	}

	public static void atualizarLinha(Processo processo) {
		
		int linha = modelo.getIndice(
				String.valueOf(processo.getID()), 0);
					
		String nome = processo.getNome();
		String prioridade = processo.getPrioridade().toString();
		String estado = processo.getEstado().toString();
		String surto = String.valueOf(processo
				.getSurto());
		String quantum = String.valueOf(processo.getQuantum());
		String tempoProcessamento = String.valueOf(processo.getTempoProcessamento());
		
		modelo.setValueAt(nome, linha, 1);
		modelo.setValueAt(prioridade, linha, 2);
		modelo.setValueAt(estado, linha, 3);
		modelo.setValueAt(surto, linha, 4);
		modelo.setValueAt(tempoProcessamento, linha, 5);
		modelo.setValueAt(quantum, linha, 6);
	}
	
	public static void addLinha(Processo processo) {
		String id = String.valueOf(processo.getID());
		String nome = processo.getNome();
		String prioridade = processo.getPrioridade().toString();
		String estado = processo.getEstado().toString();
		String surto = String.valueOf(processo
				.getSurto());
		String quantum = String.valueOf(processo.getQuantum());
		String tempoProcessamento = String.valueOf(processo.getTempoProcessamento());
		
		String[] dados = {id, nome, prioridade, estado, surto,tempoProcessamento, quantum};
		modelo.addRow(dados);
	}
	
	public static void efeitoLinha(Processo processo) {
		int linha = modelo.getIndice(
				String.valueOf(processo.getID()), 0);
		
		tabelaProcessos.setRowSelectionInterval(linha, linha);
		tabelaProcessos.setSelectionBackground(Color.BLACK);
		tabelaProcessos.setSelectionForeground(Color.GREEN);
	}
	
	public static void SetTextTempo(String tempo){
		lblTempo.setText("Tempo Total: " + tempo + " ");
	}
	
	public static void setTextQtdProcessos(String qtdProcessos) {
		lblProcessos.setText("Quantidade Processos: " + qtdProcessos + " ");
	}
	
	public static void setTextMemoria(String utilizada, String total) {
		lblMemoria.setText("Memória Utilizada: " + utilizada + "MB/" + total + "MB");
	}
}
