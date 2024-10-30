#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* sink;

// YEET
static void on_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

// setting sink on check or uncheck based on given index
static void on_check_button_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    const char *index = (const char *)user_data;
    char command[1024];
    strcpy(command, "pactl move-sink-input ");
    strcat(command, index);
    if (gtk_toggle_button_get_active(toggle_button)) {
        strcat(command, " sharedAudio");
    } else {
        strcat(command, " ");
        strcat(command, sink);
    }
    printf("%s\n", command);
    system(command);
}

int main(int argc, char *argv[]) {

    // checking to see if the sinks have already been made
    FILE *sinks;
    char path[1024];
    int found = 0;
    sinks = popen("pactl list short sinks", "r");
    while (fgets(path, sizeof(path), sinks) != NULL) {
        if (strstr(path, "sharedAudio") != NULL) {
            found = 1;
            break;
        }
    }
    pclose(sinks);

    // getting source
    FILE *file;
    file = fopen("/etc/audioShare/config.ini", "r");
    char sourceFull[256];
    fgets(sourceFull, 256, file);
    char * source = strtok(sourceFull, " ");
    for(int i = 0; i<2; i++) {
      source = strtok(NULL, " ");
    }
    if (source[strlen(source)-1] == '\n') { source[strlen(source)-1] = '\0'; }

    // getting sink
    char sinkFull[256];
    fgets(sinkFull, 256, file);
    sink = strtok(sinkFull, " ");
    for(int i = 0; i<2; i++) {
      sink = strtok(NULL, " ");
    }
    if (sink[strlen(sink)-1] == '\n') { sink[strlen(sink)-1] = '\0'; }

    // getting latency
    char latencyFull[256];
    fgets(latencyFull, 256, file);
    char * latency = strtok(latencyFull, " ");
    for(int i = 0; i<2; i++) {
      latency = strtok(NULL, " ");
    }
    if (latency[strlen(latency)-1] == '\n') { latency[strlen(latency)-1] = '\0'; }

    fclose(file); 

    // shamelessly stolen from user.dz (https://askubuntu.com/questions/421014/share-an-audio-playback-stream-through-a-live-audio-video-conversation-like-sk)
    // don't fucking ask me how this even begins to work. i'm not a sound guy, in fact i'm half deaf. boy i really should not be developing audio tools eh
    if (found == 0) {
        char command[1024];
        snprintf(command, sizeof(command),"pactl load-module module-null-sink sink_name=micAndSharedAudio sink_properties=device.description=micAndSharedAudio && pactl load-module module-null-sink sink_name=sharedAudio sink_properties=device.description=sharedAudio && pactl load-module module-loopback latency_msec=%s sink=micAndSharedAudio source=%s && pactl load-module module-loopback latency_msec=%s sink=micAndSharedAudio source=sharedAudio.monitor && pactl load-module module-loopback latency_msec=%s sink=%s source=sharedAudio.monitor && pactl set-default-source micAndSharedAudio.monitor && pactl set-default-sink %s", latency, source, latency, latency, sink, sink);
        system(command);
    }

    // setting up window
    gtk_init(&argc, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "audio share");
    GdkPixbuf *icon_pixbuf = gdk_pixbuf_new_from_file("/etc/audioShare/icon.png", NULL);
    gtk_window_set_icon(GTK_WINDOW(window), icon_pixbuf);
    g_object_unref(icon_pixbuf);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // get shared audio sink index
    char line[256];
    char sinkNumber[5];
    FILE *sinkList = popen("pactl list sinks short", "r");
    while (fgets(line, sizeof(line), sinkList) != NULL) {
            if (strstr(line, "sharedAudio") != NULL) {
                sscanf(line, "%s", sinkNumber);
            break;
        }
    }
    for (int i = 0; i < 5; i ++) {
        if (!isdigit(sinkNumber[i])) {
                sinkNumber[i] = '\0';
                break;
        }
    }
    sinkNumber[4] = '\0';
    pclose(sinkList);

    // get all application outputs
    FILE *applications;
    char buffer[1024];
    applications = popen("pactl list sink-inputs", "r");
    char appName[1024];
    int nameIndex = 0;
    char appIndex[5];
    char currentSink[256];
    while (fgets(buffer, sizeof(buffer), applications) != NULL) {
        char *ptr = buffer;
        sscanf(buffer, " Sink Input #%s", appIndex);
        sscanf(buffer, " Sink: %s", currentSink);

        while ((ptr = strstr(ptr, "application.name = \"")) != NULL) {
            ptr += strlen("application.name = \"");
            while (*ptr != '\"' && *ptr != '\0') {
                appName[nameIndex] = *ptr++;
                nameIndex++;
            }
            appName[nameIndex] = '\0';
            
            // make check box with application name and pass through index
            GtkWidget *checkBox = gtk_check_button_new_with_label(appName);
            if (strcmp(currentSink, sinkNumber) == 0) { gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkBox), TRUE); }
            gtk_box_pack_start(GTK_BOX(box), checkBox, FALSE, FALSE, 2);
            for (int i = 0; i < 5; i ++) {
                if (!isdigit(appIndex[i])) {
                    appIndex[i] = '\0';
                    break;
                }
            }
            appIndex[4] = '\0';
            g_signal_connect(checkBox, "toggled", G_CALLBACK(on_check_button_toggled),  g_strdup(appIndex));
            appName[0] = '\0';
            nameIndex = 0;
        }
    }
    pclose(applications);

    // start GTK loop
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
