package model;

public class BlocoMemoria {
	public static final int MB = 4;
	private int id;
	private boolean vazio;
	
	public BlocoMemoria(int id) {
		this.id = id;
		vazio = false;
	}
	
	public String toString() {
		return "ID : " + id + " vazio?: " + vazio + " | "; 
	}
	public boolean isVazio() {
		return vazio;
	}
	public int getID() {
		return id;
	}
}
