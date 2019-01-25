package model;

import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.table.AbstractTableModel;

public class ModeloTabelaProcessos extends AbstractTableModel{
	private static final long serialVersionUID = 1L;
	
	private ArrayList<String[]> linhas;
	private String[] colunas;
	
	public ModeloTabelaProcessos(ArrayList<String[]> linhas, String[] colunas) {
		this.linhas = linhas;
		this.colunas = colunas;
	}
	
	
	public void addRow(String [] dadosLinha){  
	    getLinhas().add(dadosLinha);  
	    // Informa a jtable de que houve linhas incluidas no modelo  
	    // COmo adicionamos no final, pegamos o tamanho total do modelo  
	    // menos 1 para obter a linha incluida.  
	    int linha = getLinhas().size()-1;  
	    fireTableRowsInserted(linha,linha);  
	    return;  
	}  
	
	/** 
	 * Remove a linha pelo valor da coluna informada 
	 * @param val 
	 * @param col 
	 * @return 
	 */  
	public int getIndice(String val, int col){  
	   Iterator<String[]> i = getLinhas().iterator(); 
	    int linha = 0;  
  
	    while(i.hasNext()){  
	        String[] linhaCorrente = (String[]) i.next();  
	          
	        if( linhaCorrente[col].equals(val) ){    
	            return linha;                  
	        }
	        
	        linha++;
	    }  
  
	    return -1;  
	}  
	
	@Override
	public int getColumnCount() {
		return colunas.length;
	}

	@Override
	public String getColumnName(int columnIndex) {
		return colunas[columnIndex];
	}

	@Override
	public int getRowCount() {
		return linhas.size();
	}

	@Override
	public Object getValueAt(int rowIndex, int columnIndex) {
		String[] linha = (String[]) linhas.get(rowIndex);
		
		return linha[columnIndex];
	}

	@Override
	public boolean isCellEditable(int rowIndex, int columnIndex) {
		return false;
	}

	@Override
	public void setValueAt(Object value, int rowIndex, int columnIndex) {	
		String[] linha = (String[]) linhas.get(rowIndex);
		linha[columnIndex] = (String) value;
		
		fireTableCellUpdated(rowIndex, columnIndex);
	}

	public ArrayList<String[]> getLinhas() {
		return linhas;
	}

	public void setLinhas(ArrayList<String[]> linhas) {
		this.linhas = linhas;
	}

	public String[] getColunas() {
		return colunas;
	}

	public void setColunas(String[] colunas) {
		this.colunas = colunas;
	}

}
