package controller;

import model.Processo;

public interface AbstractEscalonador {
	void suspender();

	Processo getProximoProcesso();

	void escalona();
}

