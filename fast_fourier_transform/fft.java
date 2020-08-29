import java.lang.Math;
import java.util.Scanner;

class Complex {
    double real, imag;

    // Constructor
    public Complex(double real, double imag){
        this.real = real;
        this.imag = imag;
    }

    // Basic arithmetic 
    Complex add(Complex other){
        return new Complex(this.real + other.real, this.imag + other.imag);
    }
    Complex subtract(Complex other){
        return new Complex(this.real - other.real, this.imag - other.imag);
    }
    Complex multiply(Complex other){
        return new Complex(
            this.real * other.real - this.imag * other.imag,
            this.real * other.imag + this.imag * other.real);
    }
}

class FFT {

    static protected int reverse_bit(int index, int log){
        int s = 0;
        for(int b=0; b<log; b++) if((index & (1<<b)) > 0)
            s |= (1 << (log - 1 - b));
        return s; 
    }

    static public void fft(Complex[] poly, boolean invert){

        // Resize
        int log = 1;
        while((1<<log) < poly.length){ log++;}

        // Index swap
        for(int i=0; i<poly.length; i++){
            int j = reverse_bit(i, log);
            if(i < j){
                Complex temp = poly[i];
                poly[i] = poly[j];
                poly[j] = temp;
            }
        }

        // Calculate
        int invertfactor = 2;
        if(invert){ invertfactor *= -1;}
        for(int l=2; l<=poly.length; l<<=1){
            double angle = invertfactor * Math.PI / l;
            Complex wfactor = new Complex(Math.cos(angle), Math.sin(angle));
            for(int offset=0; offset<poly.length; offset+=l){
                Complex w = new Complex(1, 0);
                for(int i=0; i<l/2; i++){
                    Complex front = poly[offset+i];
                    Complex back = w.multiply(poly[offset+i+l/2]);
                    poly[offset+i] = front.add(back);
                    poly[offset+i+l/2] = front.subtract(back);
                    w = w.multiply(wfactor);
                }
            }
        }

        if(invert) for(int i=0; i<poly.length; i++){
            poly[i].real /= poly.length;
            poly[i].imag /= poly.length;
        }
    }

    static public long[] multiply(long a[], long b[]){

        // Resize
        int n=1;
        while(n < a.length + b.length) n <<= 1;
        Complex poly1[] = new Complex[n], poly2[] = new Complex[n];
        for(int i=0; i<a.length; i++) poly1[i] = new Complex(a[i], 0);
        for(int i=a.length; i<n; i++) poly1[i] = new Complex(0, 0);
        for(int i=0; i<b.length; i++) poly2[i] = new Complex(b[i], 0);
        for(int i=b.length; i<n; i++) poly2[i] = new Complex(0, 0);

        // FT -> Convolution -> IFT
        fft(poly1, false); fft(poly2, false);
        for(int i=0; i<n; i++) poly1[i] = poly1[i].multiply(poly2[i]);
        fft(poly1, true);

        // Return
        long result[] = new long[n];
        for(int i=0; i<n; i++) result[i] = (long)Math.round(poly1[i].real);
        return result;
    }
}