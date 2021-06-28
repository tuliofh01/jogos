import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Janela extends JFrame {

    // Declaracao variaveis dos componentes
    JPanel painelSuperior, painelCentral, painelCampos, painelTeclado, painelInferior;
    JLabel boneco, textoPontuacao, valorPontuacao, labelTeclado;
    JTextField[] campos;
    JTextField textoTeclado;
    JButton botaoConfirmar, botaoPular, botaoReiniciar;

    // Variavel palavra
    String palavra = Controle.sortearPalavra();

    public Janela() {

        /* Configuracoes da janela */

        // Titulo da janela
        super("Jogo da Forca");
        // Layout janela
        setLayout(new BorderLayout());

        /* Configuracao das variaveis dos componentes */

        // Painel superior
        painelSuperior = new JPanel();
        painelSuperior.setLayout(new BorderLayout());
        // Boneco
        boneco = new JLabel();
        boneco.setIcon(new ImageIcon("media/1.gif"));
        painelSuperior.add(boneco, BorderLayout.NORTH);
        // Texto pontuacao
        textoPontuacao = new JLabel("PONTOS:");
        painelSuperior.add(textoPontuacao, BorderLayout.CENTER);
        // Valor pontuacao
        valorPontuacao = new JLabel("0");
        painelSuperior.add(valorPontuacao, BorderLayout.SOUTH);
        add(painelSuperior, BorderLayout.NORTH);

        // Painel central
        painelCentral = new JPanel();
        painelCentral.setLayout(new BorderLayout());
        // Campos de texto (letras)
        painelCampos = new JPanel();
        painelCampos.setLayout(new FlowLayout());
        campos = new JTextField[6];
        for (int i = 0; i < 6; i++) {
            campos[i] = new JTextField("!", 1);
            campos[i].setEditable(false);
            painelCampos.add(campos[i]);
        }
        painelCentral.add(painelCampos, BorderLayout.NORTH);
        // Teclado (input)
        painelTeclado = new JPanel();
        painelTeclado.setLayout(new FlowLayout());
        labelTeclado = new JLabel("Digite UMA letra:");
        painelTeclado.add(labelTeclado);
        textoTeclado = new JTextField(10);
        textoTeclado.addActionListener(new HandlerBotaoConfirma());
        painelTeclado.add(textoTeclado);
        painelCentral.add(painelTeclado, BorderLayout.SOUTH);
        add(painelCentral, BorderLayout.CENTER);

        // Painel inferior
        painelInferior = new JPanel();
        painelCentral.setLayout(new FlowLayout());
        // Botao confirmar
        botaoConfirmar = new JButton("Confirmar");
        botaoConfirmar.addActionListener(new HandlerBotaoConfirma());
        painelInferior.add(botaoConfirmar);
        // Botao reiniciar
        botaoReiniciar = new JButton("Reiniciar");
        botaoReiniciar.addActionListener(new HandlerBotaoReiniciar());
        painelInferior.add(botaoReiniciar);
        // Botao pular
        botaoPular = new JButton("Pular");
        botaoPular.addActionListener(new HandlerBotaoPular());
        painelInferior.add(botaoPular);
        add(painelInferior, BorderLayout.SOUTH);


        /* Configuracoes da janela */

        // Tamanho janela
        setSize(300, 450);
        // Visibilidade janela
        setVisible(true);

    }

    /* Handlers */

    private class HandlerBotaoConfirma implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            String letraSubmetida = Controle.primeiroElemento(textoTeclado.getText());
            boolean acerto = false;
            int total = 0;
            for (int i = 0; i < palavra.length(); i++) {
                if (letraSubmetida.equals(Character.toString(palavra.toCharArray()[i]))) {
                    campos[i].setText(letraSubmetida);
                    acerto = true;
                }
                if (campos[i].getText().equals("!")) {
                    total++;
                }
            }
            if (!acerto) {
                char[] pathNovoIcone = boneco.getIcon().toString().toCharArray();
                int numeroForca = Character.getNumericValue(pathNovoIcone[6]);
                boneco.setIcon(new ImageIcon(String.format("media/%d.gif", numeroForca + 1)));
                if (numeroForca >= 7) {
                    JOptionPane.showMessageDialog(null, "perdeu!");
                    System.exit(0);
                }
            }
            if (total == 0) {
                JOptionPane.showMessageDialog(null, "ganhou!");
                valorPontuacao.setText(Integer.toString(Integer.parseInt(valorPontuacao.getText()) + 1));
                palavra = Controle.sortearPalavra();
                for (int i = 0; i < 6; i++) {
                    campos[i].setText("!");
                }
                boneco.setIcon(new ImageIcon(String.format("media/%d.gif", 1)));
            }
            textoTeclado.setText("");
        }
    }
    private class HandlerBotaoPular implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            palavra = Controle.sortearPalavra();
            for (int i = 0; i < 6; i++) {
                campos[i].setText("!");
            }
            boneco.setIcon(new ImageIcon(String.format("media/%d.gif", 1)));
            textoTeclado.setText("");
        }
    }
    private class HandlerBotaoReiniciar implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            palavra = Controle.sortearPalavra();
            for (int i = 0; i < 6; i++) {
                campos[i].setText("!");
            }
            boneco.setIcon(new ImageIcon(String.format("media/%d.gif", 1)));
            textoTeclado.setText("");
            valorPontuacao.setText("");
        }
    }

}
