package model;

public interface AbstractFilaProcesso <E>{
	boolean vazia();
	
	void add(E e);
	
	E remover();
	
	E head();
}
