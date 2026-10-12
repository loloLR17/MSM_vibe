# M015-Q — critères avant extension et périmètre

Le contrôle immuable P (BIN ed19310485d51af22b18420ea8af3ff3acba36531b3e9741bc78e91792f282f5) a été réexécuté trois fois : 263 XYZ +33TS à chaque capture, après reset MCU GDB puis reset logiciel capteur interne ST. Pas de flash du témoin, pas de coupure d'alimentation ni manipulation.

## Différence logicielle unique pour les fenêtres

La fonction ST générée et les écritures d'initialisation restent identiques. À sa première demande FIFO_STATUS, le port appelle fifo_window_run et sort ensuite par le cleanup existant. La boucle impression/paquets ST est remplacée par un service segmenté. CPU16MHz/SPI2MHz/mode3/HAL_SPI O2, transport commande puis RX et configuration restent identiques au contrôle. Le nouveau module seul est O2 pour borner le coût de parsing ; aucun driver vendor ni fichier primaire modifié. BENCH_WINDOW_XYZ=0 conserve un BIN byte-identique P après build de contrôle de non-régression.

Les deux images 512/4096 ne diffèrent que par la cible XYZ. Pas de retouche entre les deux essais. Le passage à4096 exige qualification indépendante du512 et cleanup correct ; arrêt immédiat sur refus, sans nouvelle variante.

## Critères quantifiés et capacités

- Cible exacte512 puis4096 **XYZ** ; les timestamps sont conservés et comptés séparément. Dernier segment <=nombre de XYZ restant : aucun XYZ supplémentaire ni troncature silencieuse. Tout burst reçu est conservé même en cas de rejet de parsing.
- Segments <=128 mots/896 octets, sous la limite SPI3. Drain dès disponibilité du segment ; pas d'attente watermark imposée entre segments et pas de console dans la fenêtre active. Watermark capteur256, stream/DEC8/timestamps inchangés.
- RAM mots4736 =4096+512TS+128marge (33152octets), sans supposer4096 mots dans le FIFO capteur. Budget transport inchangé :1024traces/65536octets/2048snapshots, réservecleanup12/32/24. BSS ARM309456 et data476, heap512+stack2048 réservés sur786432octets : marge474000 environ.
- Statut brut dix bits avant et après chaque segment ; refus FULL/OVR/latch (mask68h) et niveau>=512 conservateur. Maximum400lectures de statut et500ms ; contrôle stop à chaque boucle. Au premier refus, préserver bruts et cleanup. Niveau initial/max/final et flags_OR publiés ; pas de garantie de perte globale zéro.
- Config définie avant premier drain puis après chaque segment :07..0A=00/01/0A/86, WHO7B, CTRL1AC, CTRL3=44, CTRL4..8=0, CTRL10=20. Les adresses11 et18 réservées restent brutes et ne constituent pas des critères de configuration.
- TagsXYZ2/TS4 uniquement, parité paire, TAG_CNT XYZ modulo4 continu, timestamps positifs monotones modulo32bits et huit XYZ entre timestamps ; pas de composante saturée. Analyse hors ligne indépendante des compteurs firmware, exactitude CSV/raw/traces et buffer RAM, aucun mot inventé.
- Transport :HAL_OK/error0/transfert complet, snapshots avant fermeture avec offsets/restants/CS/OVR conformes ; overflow/refus journal0. Mesure DWT des durées et tick de fenêtre, débit publié avec sa portée. Pas de mesure indépendante SCK/ODR ou d'excitation externe.
- Cleanup contrôlé :XLoff/FIFObypass/TSoff, CFSR/HFSR0, UIDsecond/detach. Primaire/M014/FRAM/SD/Modbus exclu.

## Sources et corrections préparatoires

[ST DS12569 Rev8](https://www.st.com/resource/en/datasheet/iis3dwb.pdf), table8 :11 et18 réservés ; §4.3/9.26–27 pour FIFO/statuts et §9.39 pourtags. Lecture constructeur effectuée dans Q. Le FIFO embarque3KB ; les grandes fenêtres sont accumulées en RAM en le drainant.

L'évaluateur répétabilité initial comparait indûment18(E0) àP(E2) : premier refus offline conservé et corrigé, cible result0. Le premier mock fenêtre attendait le timestamp **après** le dernier XYZ non encore reçu ; correction des comptes exacts575/4607 (63/511TS) pour ce flux synthétique, sans changer l'algorithme ni les contrôles.

## Validation logicielle avant cible

CTest fenêtre ciblé réussi :512/4096 synthétiques exacts, capacité/timeout/stop, overflow avant/après, corruption config/parité/counter/tag/saturation/timestamp/ratio,39positions de défaut IO, rétention des octets rejetés et tick wrap. Quatorze scénarios d'intégration utilisent la vraie fonction ST générée et le vrai port : succès et refus IO/overflow/parité/config/logcapacity/cleanup pour les deux cibles. Trois builds ARM (contrôle/512/4096) réussis ; BINcontrôle identiqueP. Warnings newlib usuels conservés. Les validations anciennes du cœur TR2 ne sont pas rejouées : expérimentation isolée seulement.
