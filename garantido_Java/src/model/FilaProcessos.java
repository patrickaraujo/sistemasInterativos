package model;

import java.util.Iterator;
import java.util.PriorityQueue;
import java.util.Queue;

public class FilaProcessos implements AbstractFilaProcesso<Processo>{
	protected Queue<Processo> fila;

	public FilaProcessos() {
		fila = new PriorityQueue<>();
	}
	
	public FilaProcessos(Queue<Processo> filaEntrada) {
		this.fila = filaEntrada;
	}
	
	public int tamanho() {
		return fila.size();
	}
	
	public Processo buscar(Processo processo) {
		Iterator<Processo> i = fila.iterator();
		while (i.hasNext()) {
			Processo p = i.next();
			if (p.equals(processo))
				return p;
		}
		return null;
	}
	
 	@Override
	public boolean vazia() {
		return fila.isEmpty();
	}

	@Override
	public void add(Processo processo) {
		fila.offer(processo);
	}

	@Override
	public Processo remover() {
		return fila.remove();
	}
	
	public boolean remover(Processo processo) {
		return fila.remove(processo);
	}

	@Override
	public Processo head() {
		return fila.peek();
	}
	
	public String toString() {
		return "Fila de processos prontos, tamanho: " + fila.size();
	}
	
	public Iterator<Processo> getIterator() {
		return fila.iterator();
	}

	public Queue<Processo> getFila() {
		return fila;
	}
}
