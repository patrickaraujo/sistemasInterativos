package lottery;

import java.util.Random;

public class Lottery {
    public static void main(String[] arg) {

        Random rand = new Random();
        char[] process = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
        int[] arrival = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        boolean insert = false;
        int p = 0;
        int q=0;
        int max_tick = 0;
        int ticket[][] = new int[10][10];
        char temp[] = new char[10];
        for (int z = 0; z < 10; z++) {
            int digit = (int)rand.nextInt(30);
            arrival[z] = digit;
        }


        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 9; j++) {
                if (arrival[j] >= arrival[j + 1]) {
                    int temp2 = arrival[j];
                    arrival[j] = arrival[j + 1];
                    arrival[j + 1] = temp2;

                    char temp1 = process[j];
                    process[j] = process[j + 1];
                    process[j + 1] = temp1;
                }
            }
        int time = arrival[0];

        int[] request = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        int[] lottery = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        for (int z = 0; z < 10; z++) {
            int digit = (int)rand.nextInt(20);

            if (digit == 0) {
                digit = 5;
            }

            request[z] = digit;

        }
        for (int i = 0; i < 10; i++) {
            System.out.print("\t process : " + process[i]);
            System.out.print("\t arival : " + arrival[i]);
            System.out.println("\t request : " + request[i]);
            System.out.println("");
        }

        int quantom = rand.nextInt(8);
        if (quantom == 0)
            quantom = 5;
        System.out.println("quantom : " + quantom);
        System.out.print("\t" + "time");
        System.out.print("\t" + "request");
        System.out.print("\t" + "process");
        System.out.print("\t" + "Lottery");
        System.out.println("\t" + "ticket");

        System.out.println("---------------------------------------------------------");
        while ((request[0] + request[1] + request[2] + request[3] + 
                request[4] + request[5] + request[6] + request[7] + 
                request[8] + request[9]) > 0) {
            System.out.println("---------------------------------------------------------");
            p = 0;
            for (int i = 0; i < 10; i++) {
                if ((request[i] > 0)) /*&& (time > arrival[i])*/ {
                    lottery[i] = request[i] / 2;
                    if ((lottery[i] == 0) && (request[i] > 0))
                        lottery[i] = 1;
                    for (int z = 0; z < lottery[i]; z++) {
                        ticket[i][z] = p++;
                        max_tick = p;
                    }
                    temp[i] = process[i];                


                    System.out.print("\t" );
                    System.out.print("\t" + request[i]);
                    System.out.print("\t" + temp[i]);
                    System.out.print("\t" + lottery[i]);
                    for (int z = 0; z < lottery[i]; z++)
                        System.out.print("\t" + "::" + ticket[i][z] + "::");
                    System.out.println();
                } else if ((request[i] > 0) && (time < arrival[i])) {
                    temp[i] = process[p];
                    lottery[i] = request[i] / 2;
                    if ((lottery[i] == 0) && (request[i] > 0))
                        lottery[i] = 1;
                    for (int z = 0; z < lottery[i]; z++){
                        ticket[i][z] = p++;
                        max_tick=p;
                    }

                    if (request[i] > 0) {
                        time += quantom;
                    } else {
                        time += (request[i] + quantom);
                    }

                    System.out.print("\t" + time);
                    System.out.print("\t" + request[p]);
                    System.out.print("\t" + temp[i]);
                    System.out.print("\t" + lottery[i]);
                    for (int z = 0; z < lottery[i]; z++)
                        System.out.print("\t" + "::" + ticket[i][z] + "::");
                    System.out.println();

                }
            }

            int winner = rand.nextInt(max_tick);
            for(int i =0;i<10;i++)
                for(int z=0;z<lottery[i];z++)
                    if(ticket[i][z]==winner)
                         q=i;
            System.out.println("---------------------------------------------------------");
            System.out.println("winner :" +"<<" +winner+">>" );
        
            if ((request[q] > 0))  {
            temp[q] = process[q];
            request[q] -= quantom;


            if (request[q] > 0) {
                time += quantom;
            } else {
                time += (request[q] + quantom);
            }

            if (request[q] < 0) {
                request[q] = 0;
            }
            
            System.out.print("\t" + time);
            System.out.print("\t" + request[q]);
            System.out.print("\t" + temp[q]);
                System.out.println();
            }else if ((request[q] > 0) && (time < arrival[q])) {
                                temp[q] = process[p];
            if (request[q] > 0) {
                time += quantom;
            } else {
                time += (request[q] + quantom);
            }

            System.out.print("\t" + time);
            System.out.print("\t" + request[q]);
            System.out.print("\t" + temp[q]);
            System.out.println();
            
        }
    }
}
}
