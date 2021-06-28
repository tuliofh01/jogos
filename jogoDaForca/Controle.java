import java.util.Random;
import java.util.Scanner;
import java.io.File;

public class Controle {

    private static Random gen = new Random();
    private static File palavras = new File("palavras.txt");
    private static Scanner scan;

    public static String primeiroElemento(String string){
        char[] array = string.toCharArray();
        return Character.toString(array[0]);
    }

    public static String sortearPalavra(){
        int counter1 = gen.nextInt(130);
        int counter2 = 0;
        String palavra = "";

        try{
            scan = new Scanner(palavras);
        }
        catch (Exception e){
            System.out.println("Nao foi possivel abrir lista de palavras!");
            System.exit(0);
        }

        while (scan.hasNext() && (counter2 != counter1)){
            palavra = scan.next();
            counter2++;
        }
        return palavra;
    }

}
