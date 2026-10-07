import java.nio.file.*;
import java.util.*;

public class SistemOperasi {

    // Penghitung bersama 
    static int selesai = 0;
    static synchronized void tandaiSelesai(String nama) {
        selesai++;
        System.out.println(">> " + nama + " selesai (" + selesai + "/3)");
    }

    static void tidur(long ms) {
        try { Thread.sleep(ms); } catch (InterruptedException e) { }
    }

    // Input angka (bilangan bulat positif)
    static int bacaAngka(Scanner in, String pesan) {
        while (true) {
            System.out.print(pesan);
            try {
                int n = Integer.parseInt(in.nextLine().trim());
                if (n > 0) return n;
            } catch (NumberFormatException e) { }
            System.out.println("  Input tidak valid, masukkan angka bulat positif.");
        }
    }

    public static void main(String[] args) throws Exception {
        Scanner in = new Scanner(System.in);

        // ===== Input =====
        int batas = bacaAngka(in, "Cari bilangan prima sampai (misal. 50): ");
        System.out.print("Kata/kalimat yang mau dibalik            : ");
        String kata = in.nextLine();
        System.out.print("Teks untuk ditulis ke file lalu dibaca: ");
        String teks = in.nextLine();
        System.out.println("\n--- Thread dimulai ---");

        // Thread 1: bilangan prima sampai batas
        Thread prima = new Thread(() -> {
            List<Integer> hasil = new ArrayList<>();
            for (int n = 2; n <= batas; n++) {
                boolean p = true;
                for (int i = 2; i * i <= n; i++) if (n % i == 0) { p = false; break; }
                if (p) hasil.add(n);
                tidur(30);
            }
            System.out.println("[Prima] 1-" + batas + ": " + hasil);
            tandaiSelesai("Prima");
        });

        // Thread 2: balik kata + cek kesamaan balik kata
        Thread balik = new Thread(() -> {
            String bersih = kata.replace(" ", "").toLowerCase();
            String dibalik = new StringBuilder(kata).reverse().toString();
            boolean Sama = bersih.equals(new StringBuilder(bersih).reverse().toString());
            tidur(200);
            System.out.println("[Balik] \"" + kata + "\" -> \"" + dibalik + "\"");
            System.out.println("[Balik] Sama? " + (Sama ? "Ya" : "Tidak"));
            tandaiSelesai("Balik");
        });

        // Thread 3: tulis teks file dan baca kembali
        Thread file = new Thread(() -> {
            try {
                Path f = Paths.get("input_user.txt");
                Files.write(f, Arrays.asList(teks));
                tidur(300);
                for (String baris : Files.readAllLines(f))
                    System.out.println("[File] Isi file: " + baris);
            } catch (Exception e) {
                System.out.println("[File] Error: " + e.getMessage());
            }
            tandaiSelesai("File");
        });

        List<Thread> semua = Arrays.asList(prima, balik, file);
        for (Thread t : semua) t.start();
        for (Thread t : semua) t.join();
        System.out.println("Semua thread selesai.");
    }
}