> Plan d’essais futurs, aucune intervention matérielle autorisée dans la mission V1.2. Les gates PCB nus sont exclusivement dans PRECOMMANDE_V1_3.md ; aucune exigence de détrompage automatique, boîtier fini ou centres LED.

# Plan après réception — aucune étape matérielle exécutée

Les critères suivants restent à mesurer sous une nouvelle mission autorisant les manipulations et le debug. Ne pas transformer ce plan en résultat.

1. Revue indépendante des sources, gabarit et assemblage à blanc ; détrompage/retention effectifs, accès aux boutons et SD, vis non conductrices appropriées, absence de flexion/traction sur broches, dégagement de la barrière RS485. Bloquer si la première insertion exige un effort ou un décalage de broches.
2. Hors tension et modules retirés : contrôle à la loupe des soudures et des courts-circuits, continuité point par point de la matrice et NC, absence de liaison 3V3/5V, absence de liaison logique/GND2 isolée. Les seuils d’isolement et tensions d’essai doivent rester compatibles avec les composants ; aucun mégohmmètre appliqué arbitrairement.
3. Alimentation contrôlée, source choisie explicitement selon procédure Nucleo : vérifier 3V3/GND aux sockets, polarité, consommation, absence d’échauffement avant modules ; couper pour poser chaque module. Ne jamais déplacer JP6 ou insérer une SD sous tension.
4. Identité capteur 0x7B puis lecture/écriture contrôlée de configuration et FIFO borné, sans affaiblir contrôles timestamps/qualité. Tester la répétabilité après transport/power cycle ; consigner image, cible, longueur de nappe, fréquence et fixation.
5. SD SPI2 sur carte de test autorisée seulement : identité puis fichier inédit court, vérification intégrale indépendante. FRAM existante : lecture seule et intégrité ; aucune écriture/provisioning sans contrat spécifique.
6. RS485 : confirmer VCCSEL3V3 et pont DE-/RE existant, puis requête Modbus bornée autorisée avec propriétaire unique du port. Vérifier coexistence capteur/FIFO, SD et supervision sous charge représentative ; ne pas conclure à la coexistence sur un seul WHO_AM_I.
7. Paliers 60 s puis 10 min, vérification intégrale des données, absence d’erreurs de transport et de perte ; campagne plus longue uniquement après ces critères. Contrôle des vibrations de montage/échauffement et reprise d’effort avant installation au réducteur.

Chaque résultat devra distinguer compilation, transfert, exécution, observation GDB et qualification physique. Conserver le firmware actuellement fonctionnel et les données avant toute mission matérielle. Les anciens défauts d’écrêtage et les données OBE non récupérées ne sont pas résolus par ce shield.

## Complément V1.1 — portes et alimentation autonome

Avant tout essai en mouvement : démontrer à blanc, boîtier fermé, insertion/poussée/éjection/retrait SD sans démontage, accès conducteur et tournevis RS485, observabilité distincte des trois LED, USER sans RESET involontaire, liberté des fiches et absence d’effort sur connecteurs. Démontrer retrait/remise de chaque module sans soudure, impossibilité d’insertion inversée J8/J9 et absence de maintien de USER appuyé au repos. Vérifier les tolérances et freinages des fixations après transport/vibrations modérées, avec critères quantifiés à définir par revue.

**Avenant V1.1-A :** qualifier le montage retenu secteur externe5V→CN15 : stabilité, pics d’écriture SD et durée complète, câble réel/reprise d’effort ; aucune arrivée230V dans TR2. Les powerbanks et l’alimentation du produit final sont différées, sans gate powerbank imposé à la CAO. Vérifier la fin propre du writer avant coupure ; aucun essai de perte de puissance ou corruption de média autorisé ici. Contrôler le bilan3V3 Nucleo+modules et la chute aux pics avant qualification simultanée SD/FRAM/RS485/FIFO.
