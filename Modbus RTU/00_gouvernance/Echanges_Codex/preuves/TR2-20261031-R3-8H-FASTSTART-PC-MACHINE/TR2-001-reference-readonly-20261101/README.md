# Comparatif de référence TR2-001 — lecture/debug autorisés, résultat positif borné

## Autorisation humaine et portée

L'utilisateur demande de reprendre TR2-001 avec son code et sa configuration pour distinguer module capteur et banc TR2-002. Après proposition précise, il répond **« oui je l'autorise pour ce test »** : exception documentée à l'interdiction TR2-001 du contrat, limitée à lecture/debug pour identité/firmware/état, **sans flash, reset, START/STOP envoyé ni écriture FRAM**. MISSION_EN_COURS.md reste propriété ChatGPT et n'a pas été réécrit. Les autres limites de la mission et protections restent applicables.

L'utilisateur confirme ensuite : **« Capteur remis sur TR2-001 hors tension ; TR2-002 arrêté »**, avec son câblage d'origine suivant la question posée. Aucun transfert sous tension demandé, aucun changement de fil ou de module effectué par Codex. Identité physique du module fondée sur cette confirmation, WHO n'étant pas un identifiant unique.

## Séquence exacte

1. Git local/origin main propres et synchronisés à `1f03cc94e77efa49d9beea3a24e2f2eaf68eaa04`, deux stashes historiques préservés. Aucune compilation ni suite de tests rejouée.
2. Première liste ST-LINK :002 encore présent. Préparation d'une lecture de sûreté002 avant demande de transfert ; entre-temps le branchement change. Serveur refuse SN002 absent, GDB ne se connecte pas, aucun nouvel état002 obtenu et aucun fallback vers001. Logs d'échec conservés. Liste suivante :001 attendu présent. L'arrêt002 est ensuite confirmé par utilisateur, pas présenté comme nouvelle observation GDB.
3. Lecture001 via chaîne GDB qualifiée, port23336, SN003500463235510B37333439, UID004A003D/38425018/20333157 vérifiés. Aucun CubeProgrammer HOTPLUG, reset, chargement d'image ou appel de fonction cible. Seuls registres, RAM et Flash lus.
4. Flash150976octets lue **strictement identique** à l'image opérateur existante source `72a7b57381f61e9f24c6bdcd20dd70a38dbc70de`, SHA-256 `e1846c87017fd7667daf1b79cb440af1024346c5deae8d5065f8525c00aa7b80`. ELF local vérifié SHA6dddfaaf…35b012 avant interprétation des symboles. Aucune prétention de contrôle des2Mio entiers.
5. Relevé de la calibration **déjà exécutée automatiquement au démarrage par le firmware conservé**, après remise sous tension utilisateur. Codex ne la relance pas et n'envoie aucune commande d'acquisition. Les accès GDB se font après son achèvement. L'absence de START envoyé ne signifie pas absence d'acquisition diagnostique interne au boot.

## Observations physiques bornées et contre-calcul hors ligne

- Configuration durable active **config3/génération3/plage code0=±2g**, ODR nominal26667Hz. SPI3 nominal4MHz/mode3/duplex, CFG1=10070007 / CFG2=87400000. Ces deux registres sont également ceux conservés du candidat N8E4MHz sur002 ; cela ne rend pas identiques les deux logiciels ni les conditions matérielles.
- Calibration nominale2,000195s : **52442XYZ /58998mots FIFO /6556ancres /195brackets** ; erreur0/counter_mismatch0/reserved_nonzero0. Pas de preuve SI de durée, ni étalonnage d'amplitude.
- Résidu maximal **23µs**, limite100µs conservée. Recalcul indépendant à partir des195brackets : **23µs exacts**, spans1894603µs/149780ticks identiques. Script `analyze_boot.py`, code0/assertions satisfaites. C'est une analyse des preuves réelles, pas une répétition du test physique.
- Préfixe2048mots réellement conservé : **1820XYZ et228timestamps**, parité de tous les tags correcte,227intervalles complets de8XYZ, aucun rail dans ce préfixe. Axes variables (332/185/375 valeurs distinctes) ; pas qualification RMS/crêtes, pas de zéro défaut global déduit de ce préfixe seul. Tous les52442XYZ ne sont pas disponibles bruts.
- Transactions de fin de calibration conservées : **00 01 0A 86**, **7B A0 00 44…**, timestamp_enable20 ; à l'arrêt FIFO_CTRL4 reste80, puis écriture80/lecture80 cohérentes. Le bit qui disparaissait sur002 est donc bien présent sur001 dans cet essai. Les27transactions retenues sont toutes valides selon le transport ; elles ne représentent pas l'intégralité du boot.

## Conclusion proportionnée

Le module emprunté fonctionne sur son banc TR2-001 pour configuration/lecture FIFO/timestamps/calibration de démarrage. Une panne totale ou une incapacité permanente de ce module à conserver le champ timestamps n'explique donc pas tous les échecs002. **Le défaut se situe dans les différences entre ensembles ou conditions testés ; la Nucleo002 seule n'est pas condamnée.** Câblage, alimentation, logiciel, état initial et plage±2g/±8g restent des facteurs non isolés. Le capteur initial002 n'a pas encore été testé ici. Aucun succès ±8g, long, SD, AC1 LIVE ou supervision/Modbus revendiqué.

Prochain comparatif utile, non exécuté : capteur initial002 sur ce même ensemble001, code/config/câblage001 conservés et transfert hors tension confirmé par utilisateur. Si les deux modules passent, recentrer le diagnostic sur banc002 et logiciel/configuration ; ne pas remplacer une Nucleo sur ce seul indice. Tout START/STOP ou changement de plage, flash et opération FRAM demeure hors de l'extension actuelle et exige une autorisation distincte appropriée.

## Préservation et état final

Lecture finale : stage7/error0/work.error0, FIFO diagnostique et propriétaire arrêtés, config3/gen3/FS0, CS capteur haute. Dernier terminal durable enRAM toujours STOP5/25/run17/config3gen3, génération126/status4/result0. Le compteur RAM stops=1 provient du chemin de boot ; aucune nouvelle transaction STOP n'a été envoyée par Codex. Pas de COM4, fonction cible, accès FRAM commandé, SD, firmware/service Windows ou règle de sécurité touchés. **FRAM entière non relue/hachée pendant ce diagnostic**, donc pas de déclaration d'identité intégrale sans preuve. GDB détaché, serveurs terminés ; firmware001 conservé à l'identique, TR2-002 hors tension selon utilisateur.

Manifestes SHA des originaux, zlib sans perte et scripts inclus. Les scripts `qualified_helper_reference.py` contiennent une fonction de programmation historique jamais appelée ; commandes réellement exécutées dans les fichiers `.gdb`/invocation. Originaux locaux dans `/home/lolo/dev/msm/reports/TR2-001-reference-readonly-20261101`. Publication et relecture à consigner après push. Mission8h toujours BLOCKED sur002, ce sous-test positif ne vaut pas READY.
