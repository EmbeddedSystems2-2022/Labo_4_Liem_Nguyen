#include <stdio.h>
#include <gtk/gtk.h>
#include "PJ_RPI.h"
#include <linux/i2c-dev.h>

char output[100];
char *output_p = "17";
char outputChanged[100];
char outputToggled[100];
char showInputData[100];
char inputChanged[100];
int toggler = 0;
GtkWidget *inputData;

void toggleOutput(GtkWidget *wid, gpointer ptr)
{
    if (toggler == 0)
    {
        GPIO_SET = 1 << (int)output_p;
        toggler = 1;
        sprintf(outputToggled, "%s has been toggled ON.\n", output);
        printf(outputToggled);
    }
    else
    {
        GPIO_CLR = 1 << (int)output_p;
        toggler = 0;
        sprintf(outputToggled, "%s has been toggled OFF.\n", output);
        printf(outputToggled);
    }
}

void comboChanged(GtkEntry *e)
{
    if (map_peripheral(&gpio) == -1)
    {
        printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
    }
    sprintf(output, gtk_entry_get_text(e));
    output_p = output;
    output_p = output_p + 4;
    INP_GPIO((int)output_p);
    OUT_GPIO((int)output_p);
    sprintf(outputChanged, "Output value has changed to %s.\n", gtk_entry_get_text(e));
    printf(outputChanged);
}

void autoOutput(GtkWidget *wid, gpointer ptr)
{
}

void autoInput(GtkWidget *wid, gpointer ptr)
{
}

void showInput(GtkWidget *wid, gpointer ptr)
{
    int file_i2c;
    int length;
    unsigned char buffer[60] = {0};
    char temperature[10];

    char *filename = (char *)"/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDWR)) < 0)
    {
        printf("Failed to open the i2c bus");
        return;
    }

    int addr = 0x48;
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        return;
    }

    buffer[0] = 0x00;
    length = 2;
    if (write(file_i2c, buffer, length) != length)
    {
        printf("Failed to write to the i2c bus.\n");
    }

    length = 1;
    if (read(file_i2c, buffer, length) != length)
    {
        printf("Failed to read from the i2c bus.\n");
    }
    else
    {
        INP_GPIO((int)output_p);
        sprintf(inputChanged, "%s has changed to input mode.\n",output);
        printf(inputChanged);
        sprintf(showInputData, "TC74: %d°C\n%s: %d", buffer[0],output,GPIO_READ((int)output_p));
        gtk_label_set_text(GTK_LABEL(inputData),showInputData);
    }
}

void main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "glade-file.glade", NULL);

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    inputData = GTK_WIDGET(gtk_builder_get_object(builder, "inputData"));

    gtk_builder_connect_signals(builder, NULL);

    gtk_widget_show_all(window);

    gtk_main();
}