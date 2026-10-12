# M015-B — Audit FIFO borné et verrou observé

## Sources et configuration retenue

FIFO vérifiée dans [ST AN5444 Rev 3, §6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) et le [driver officiel ST](https://github.com/STMicroelectronics/iis3dwb-pid). La datasheet DS12569 référencée par M015-A reste la référence du mode SPI ; son nouvel accès URL a expiré. Aucun contenu non consulté n'est cité.

Registres FIFO_CTRL1..4 aux adresses 07..0A, watermark 9 bits et STOP_ON_WTM bit7 ; BDR_XL=0xA ; mode1 FIFO, mode6 continu. STATUS1 doit précéder STATUS2 avec BDU. Le niveau est sur10bits ; overrun IA bit6 et latched bit3. Latched s'efface lors de la lecture STATUS2 : chaque statut observé est conservé. Un mot de sept octets depuis0x78 contient tag+XYZ ; tag sensor2, compteur modulo4 et parité paire. Chaque batch XL incrémente le compteur.

## Prototype et critères quantitatifs

Une seule capture de256XYZ, modeFIFO1/STOP_ON_WTM=1, watermark256, SPI mode3/625kHz. Le mode FIFO arrête l'écriture à sa capacité ; la vidange se fait ensuite sans reprise avant bypass. Le contrôle négatif prévu est une attente32ms en modecontinu, au-delà de512/26667≈19,2ms, avec rejet des données écrasées. Aucun DMA, stockage, B5, pipeline de production, FFT ou excitation n'est ajouté. Aucun critère d'immobilité : le voilier peut bouger.

À625kHz, huit octets par transaction donnent au maximum9765,6mots/s avant HAL et statuts, moins que26667. La capture bornée peut contourner cette limite, pas un flux continu. BuffersRAM :256×56+512×16+188=22716octets. L'option FIFO est OFF par défaut et exclusive des autres profils/TX ; triggerunique4D313542. Driver/WHO/configuration existants réutilisés, transactionsFIFO dans le seul harness. Sept registres volatils sont sauvegardés/restaurés, y compris après erreur ; FIFO préexistante et autorité active refusées sans écriture.

Critères :256mots, niveau256→0, tagsXL/parité/255transitions compteur+1mod4, zéro overrun/erreurSPI, saturation rapportée sans rejeter une variation physique. Les bornes DWT d'activation et watermark permettent une estimation de cadence :255/(wm_high−start_low) à256/(last_not_wm_low−start_high), avec marginage de phase initiale et polling. La référence HCLK n'est pas étalonnée ; timestamps de lecture et de conversion sont distincts. Une proximité à26667 dans5% était proposée, sous réserve de statuts fiables.

Le compteur modulo4 seul ne détecte pas les pertes multiples de4. La continuité aurait été une déduction croisée entre mode arrêté, niveaux, flags et tags, pas une garantie générale. Le nombre exact de mots écrasés ne doit pas être inventé.

## Verrou physique de l'unique campagne

Code15c3369, WHO7B, mode3/625kHz et registres SPI MCU conformes. Les171polls montrent11reculs du niveau avant toute lecture FIFO, notamment127→0. Le poll169 donne0x40FE, niveau254 et flag OVR_IA ; le poll170 donne0xA100, niveau256/watermark/full. FIFO_CTRL2, demandé et vérifié0x81, est relu0x80 dans la configuration ultérieure. Ces observations sont incohérentes avec une FIFO arrêtée qui se remplit sans consommateur.

HAL ne signale aucune erreur, mais cela ne prouve pas l'intégrité des valeurs retournées. La cause peut relever des transactions/lectures SPI ou du comportement/configuration capteur ; elle n'est pas isolée. Le flag brut ne démontre pas à lui seul un écrasement physique, puisque les statuts sont incohérents.

Le garde refuse avant la vidange : count=0, result=INVALID_STATE, loss_flags=0x21. Le bit0x20 est ajouté par le garde global dès qu'une perte existe ; il ne prouve pas un mauvais niveau final, qui vaut bien256. Aucun XYZ synthétique n'est publié. Le contrôle négatif prévu n'est pas atteint. Les estimations26244..26864Hz dépendent des statuts incohérents : elles ne qualifient pas la cadence.

M015-B reste BLOCKED. La limite d'un prototype/une campagne est respectée : aucune variante SPI supplémentaire ni requalification M015-A. La prochaine mission doit d'abord établir la fiabilité des registres FIFO : readbacks répétés, ordreBDU, framing/longueurs de transfert et, si nécessaire, observation SCK/MOSI/MISO. Ce sont des recommandations, pas des essais exécutés ici.

Firmware M014 restauré exactement, Flash2Mio identique au backup, boot FRAMV2/gen12 sain, deuxFC03 B0/B1 réussies. Aucun accèsFRAM/SD dans le profil isolé, aucun provisioning ni risque sur la partitionLBA65536.

## Complément de reprise — analyse hors ligne, 12 octobre 2026

Les 171 relevés originaux restent inchangés. Les 11 reculs comptent des transitions descendantes ; ils ne comptent ni les échantillons perdus ni toutes les lectures suspectes. Le premier recul est 127→0 (indice 82), avant toute lecture de données FIFO. Un modèle exploratoire ajusté uniquement sur les indices 0..81 (médiane des pentes entre points espacés d’au moins 20 relevés, puis médiane des intercepts, temps de fin de transaction et horloge nominale 160 MHz) donne environ 26,62 kHz. **Ce résultat ne qualifie pas la cadence** : les lectures initiales ne disposent pas d’une validation indépendante et les timestamps encadrent des transactions, pas des conversions.

Avec un seuil exploratoire de trois unités, 17 lectures s’écartent du modèle, toutes après l’indice 81. Plusieurs valeurs sont compatibles avec un décalage du byte de niveau vers la gauche modulo 256 (par exemple niveau hypothétique 128→0, 130→4, 159→62) ou la perte du bit 7 (134→6, 158→30). D’autres ne suivent pas ces transformations. Les candidats utilisent une tolérance arbitraire de deux unités autour du modèle : **hypothèse de fiabilité de lecture à examiner, pas cause démontrée ni correction des données**. Calculs, méthode et résidus : `preuves/TR2-20261012-M015-B/fifo-offline-patterns.json`.

Un simple décalage entre lecture des octets bas/haut au passage 255→256 n’explique pas à lui seul le premier recul 127→0 ni les reculs précédant ce passage. La cohérence BDU/ordre des lectures reste néanmoins à établir. Le mouvement du bateau ne suffit pas à expliquer un compteur d’occupation descendant sans consommateur ; aucune immobilité n’est supposée.

L’overrun est un **bit observé** dans le statut 0x40FE à l’indice 169, suivi de 0xA100/256. Comme la chaîne de lecture des statuts est incohérente, ce bit ne permet pas de conclure à un écrasement physique ni d’en compter les pertes. Le garde a correctement refusé la capture ; aucun XYZ accepté, aucune vidange nominale ni contrôle négatif exécuté.


Conclusion : fiabilité des lectures à établir avant nouvelle qualification ; aucun diagnostic matériel relancé.
