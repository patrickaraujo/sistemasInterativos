package view;

import javax.swing.JFrame;

import controller.Escalonador;

public class TesteView {
	public static void main(String[] args) {
		TelaJFrame tela = new TelaJFrame();
		tela.setVisible(true);
		tela.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		tela.setBounds(400, 50, 590, 600);
		
		Escalonador escalonador = Escalonador.getInstancia();
		escalonador.escalona();
	}
}
