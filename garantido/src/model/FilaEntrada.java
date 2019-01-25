package model;

import java.util.LinkedList;
import java.util.Queue;

public class FilaEntrada extends FilaProcessos {
	Queue<Processo> fila;
	
	public FilaEntrada(){
		fila = new LinkedList<Processo>();
	}
	
	public Processo add(int surto) {
		Processo processo = new ProcessoFactory().criaProcesso(surto);
		fila.offer(processo);

		return processo;
	} 
	
	public Processo remover() {
		return fila.remove();
	}
	
	public Processo head() {
		return fila.peek();
	}
	
	public int tamanho() {
		return this.fila.size();
	}
	
	public boolean vazia() {
		return fila.isEmpty();
	}
	
	public String toString() {
		return "Fila de Entrada, tamanho: " + fila.size(); 
	}
}
