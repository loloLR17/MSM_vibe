# Incidents d'outillage et corrections

- Le premier correcteur supposait l'ancien formatKiCad à codes numériques de nets ; le PCB réelKiCad10 utilise des noms. `KeyError` constaté avant toute sauvegardePCB, lecteur corrigé à partir de l'expressionnative réelle. Schéma/bibliothèque du candidat seulement avaient été écrits ; génération reprise depuis la sourceV2.3 immutable.
- Deux segments de la vieille descriptionroutesJSON diffèrent des coordonnées natives de1nm. La sélection des54segments terminaux emploie une résolution10nm pour identifier ces segments ; **aucune géométrie n'est arrondie dans le PCB** et M-01 compare ensuite les coordonnées natives exactement. Les4vias sont conservés.
- Le vérificateur de symboles a d'abord comparé le nom brut au `pinfunction`XML. KiCad10 exporte `nom_numéro` et ajoute `+no_connect` au type des broches marquéesNC. Les sorties réelles ont été inspectées ; la comparaison exacte intègre maintenant ces conventions et les180pins sont conservées.
- SchémaST officiel lisible viaoutilWeb ; téléchargementdirect Python expiré, puiscurlWindows expiré (0octet) ; pas de politique/proxy/TLS changé, pas de fichier officiel local prétendu. Les métadonnées des deuxéchecs sont conservées à côté de la transcription hachée.
- Quelques pages commercialesDigiKey/Mouser retournent403/redirectindisponible après aperçu partiel. Aucune connexioncompte ni contournement ; stocks/prix non vérifiés restent explicitement inconnus. Le CSV de préparation initial a été normaliséLF avant publication ; ce ne sont pas des journaux bruts altérés.

Aucune de ces corrections ne masque unERC/DRC raté : les premières exécutions natives ont donné0violation. Aucun ancien testphysique ou firmware n'a été rejoué.


## Vérification de publication avant clôture

La première publication complète provisoire `1078d465` ne contenait pas le ZIP CAM : la règle globale `*.zip` de `.gitignore` l'avait exclu de l'ajout du dossier. Le contrôle de concordance inventaire/Git a identifié cette absence avant clôture. Seul le ZIP candidat exact est ajouté explicitement, sans changer les règles d'exclusion ni régénérer les CAM. Son hash reste `d2a199a1bf3b9d9533bfe788b144510bb8cfd518dc248f05df2c2a644854e7ec`. La relecture distante doit ensuite ouvrir réellement ce ZIP et comparer les neuf membres.

La synthèse incluait aussi le fichier local `.kicad_prl`, état d'interface graphique ignoré qui ne fait pas partie des sources évaluées à publier. Cette entrée a été retirée de la liste des sources et du manifeste des livrables ; le fichier local est conservé. Aucun résultat de validation CAO ni octet du PCB/CAM n'est modifié.
