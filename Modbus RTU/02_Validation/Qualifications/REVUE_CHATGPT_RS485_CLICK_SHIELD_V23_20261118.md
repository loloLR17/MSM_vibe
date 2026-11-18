# Revue indépendante ChatGPT — chemin RS-485 Click, shield V2.3, TR2-001

Date : 2026-11-18. Référence GitHub : `main`, publication Codex `2eefbb7e0cff1f1c8c72aa3e38dd495b1d6eddf0`, clôture `9bc43dd3df56a056e3deed9ff0705536bee5f051`.
Mission : `TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL`, rapport Codex `DERNIER_RAPPORT.md` statut `DONE`.

## Verdict indépendant

**QUALIFICATION FONCTIONNELLE PHYSIQUE BORNÉE du chemin RX/TX/Modbus RTU RS-485 Click sur le seul assemblage TR2-001**, à 115200 bit/s, 8E1, avec ST-LINK et adaptateur USB–RS485 T1000. Aucun GO pour shield complet, remise sous tension du MKIGIBV2 ou refabrication V2.3, dont le défaut SPI3 J8 reste bloquant.

## Preuves consultées directement sur GitHub

- Journal `native-RAW-01.jsonl` : 8 séquences 0..7 avec réponses de 26 octets portant nonce, séquence et UID, différentes des requêtes.
- Journal `native-MODBUS-01.jsonl` : 8 transactions FC03, requête `01 03 03 E8 00 01 04 7A`, réponse `01 03 02 00 00 B8 44`, réponses hôte marquées exactes.
- `independent-wire-RAM-verification.json` : comparaison hôte/RAM pour RX (14 octets), RAW (8 échanges) et Modbus (8 échanges), 16 transitions DE haut/bas cohérentes, erreur cible zéro, zéro TX pendant RX seul.
- Recalcul direct indépendant des CRC16 Modbus sur trois trames : requête `0x7A04`, réponse `0x44B8`, exemple RAW `0x69BB` : concordance avec les deux octets de fin enregistrés.
- Rapport Codex : continuité utilisateur CN12-69 PG4 → CS/DE Click 0,3 Ω, J5 monté, niveaux 3,3 V TX et 0 V DE repos, rails 3,3 V, configuration half-duplex et VCCSEL déclarées ; UID cible `004A003D/38425018/20333157`, SN ST-LINK `003500463235510B37333439`. Code cible `811a3eda214ffb9c6c3dfbacd2af2884d87143c4`, outils hôte `73c3d252ccc534576dc72cba1414d4a0312e7dd4`, empreinte BIN `72e5ab65456a9743e3f0bcfd5d33d751b9daa05792a47be46197e277682f4563`.

## Portée et limites

Les journaux proviennent de l'exécution Codex et les constats de niveaux/continuités de l'utilisateur ; ChatGPT n'a pas personnellement instrumenté le banc ni rejoué les 17 échanges. La lecture directe des journaux, le CRC recalculé et l'invariant nonce/UID/numéros de séquence étayent un passage réellement bidirectionnel et non un simple loopback. Le statut `DONE` du contrat et le contrôle documentaire ne valent pas qualification industrielle.

Ne sont **pas qualifiés** : second shield TR2-002, tenue EMC/endurance/câbles longs, isolation galvanique haute tension, marges VDDIO2/3,3 V en charge, terminaison/bias du bus dans d'autres topologies, révisions matérielles Click non inspectées à neuf, et fonctionnement intégré avec tous les breakouts. J8 MKIGIBV2 comporte toujours six écarts électriques au sens E-01. Les autres breakouts étaient retirés pendant l'essai.

## Conséquence pour correction PCB

Le **routage logique RS-485 J9/PG4/LPUART** est un acquis à préserver dans la variante corrective. Il ne faut ni refondre cette interface ni imposer un nouvel essai identique sans modification pertinente. Pour une nouvelle fabrication : produire variante KiCad distincte, corriger MKIGIBV2 (CS19/SCL20/SDA21/SDO22), respecter E-01 et comparer KiCad/netlist/CAM nouveau contre références constructeur ; contrôler les changements réels sur tous les pads avant tout GO. Le fichier Gerber V2.3 historique **ne doit pas être renvoyé au fabricant**.

Aucune création de tag jalon (pas de jalon Mnnn ni de qualification produit complet). Aucune commande, flash, réparation ou nouvelle génération CAM exécutée lors de cette revue.
