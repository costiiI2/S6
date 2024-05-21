\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 8:Device Tree overlay**

\hfill\break

\hfill\break

Département: **TIC**

Unité d'enseignement: **SCF**

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\raggedright

Auteur(s):

- **CECCHET Costantino**

Professeur:

- **DASSATTI Alberto**
- **YANN Thomas**
  
Assistant:

- **JACCARD Anthony I.**

Date:

- **15/05/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Pour ce laboratoire il a fallut mettre en place et comprendre le fonctionement des Device Tree Overlay.

## **Tuto Reds**

En suivant le tutoriel fourni par le Reds, et les fichiers fournis, du labo précédent, nous avons pu mettre en place la base pour faire fonctionner les Device Tree Overlay.

En premier lieu, nous avons ou generer le __.rbf__ a partir su __.sof__ fourni, il a fallue modifier le fichier __Makefile__ pour que le __.rbf__ soit généré correctement.

Ensuite, nous avons du modifier le fichier __.dts__ pour redéfinir le noeud de base **fpga_bridge0**.

Puis il a fallu créer le device tree overlay,

```
/dts-v1/;
/plugin/;

&base_fpga_region {
        firmware-name = "DE1_SoC_IO.rbf";
        fpga-bridges = <&fpga_bridge0>;
        de1_io {
                compatible = "de1_io";
                reg = <0xFF200000 0x1000>;
                status = "ok";
        };
};
```

ce nouveau fichier une fois ajouter au makefile, permet de générer le __.dtbo__, une fois les deux fichier généré, il faut les copier sur la carte SD, et les charger sur la carte.

## **Deploiement sur DE1-SoC**

Pour verifier le bon fonctionnement, il faut tout d'abord s'assurer de la position des switchs ( tous a 0), puis copier la zImage, le device tree, le device tree overlay et le .rbf (dans le dir /lib/firmware) sur la carte SD.

En suivant le tutoriel, on peux s'assurer que la carte soit prête a recevoir les device tree overlay.

```bash
cat /sys/class/fpga_manager/fpga0/state 
```
cette commande permet de verifier que le fpga est bien en mode __power off__.

puis on peux executer la commande suivante pour charger le device tree overlay.

```bash
mount -t configfs none /sys/kernel/config
mkdir -p /sys/kernel/config/device-tree/overlays/<overlay_binary>
echo <overlay_binary>.dtbo > /sys/kernel/config/device-tree/overlays/<overlay_binary>/path
```

Le système devrait alors charger le device tree overlay, et le fpga devrait être en mode __operating__.


Plus qu'a tester le bon fonctionnement du device tree overlay comme pour les laboratoires précédents.

Si nous voulons retirer le device tree overlay, il suffit de faire la commande suivante:

```bash
rmdir /sys/kernel/config/device-tree/overlays/<overlay_binary>
```

## **Conclusion**

Ce laboratoire nous a permis de comprendre le fonctionnement des device tree overlay, et de les mettre en place sur la carte DE1-SoC.

\pagebreak