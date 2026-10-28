# MSM TR2 — R0-MÉTIER — Cahier des charges fonctionnel V1

**Statut : REVUE EN COURS — DOCUMENT DE TRAVAIL NON GELÉ.** Date 29/10/2026. Rédaction sous responsabilité ChatGPT sur `main`. Distinct du **R0 historique LIVE** du 15/10. Les spécifications V1/V1.1 et LIVE R0 historiques restent inchangées.

## 1. Finalité validée par l'utilisateur

**TR2 = enregistreur autonome de vibrations structurelles XYZ**, placé à environ six endroits stratégiques du navire (PC machine, moteurs, aménagements passagers, passerelle, structures). Mesurer objectivement ce qui est aujourd'hui surtout ressenti ; permettre des comparaisons a posteriori entre manœuvres, démarrages/arrêts moteurs, propulseurs, régimes PTO, etc. Finalité : **confort passagers et préservation du matériel**. Les mouvements lents de roulis/tangage et la recherche de défaillance de roulements à haute fréquence ne sont pas prioritaires. Le TR2 n'est pas un système de protection ou d'arrêt de sécurité certifié.

L'enregistrement local continu, indépendant du PC et de la connexion RS-485, **prime** sur les améliorations de supervision LIVE et les analyses embarquées.

## 2. Exigences du produit V1

| ID | Exigence | Preuve de réception attendue |
|---|---|---|
| V1-01 | XYZ **continus**, sans temps mort volontaire entre fenêtres | Chronologie, indices, nombre total d'échantillons et tous les trous/overflows explicites ; aucune perte silencieuse |
| V1-02 | Enregistrement **local durable et autonome de 8 h minimum** | Huit heures capturées, relues et comparées, avec métadonnées, CRC/qualité et incidents documentés |
| V1-03 | Signal XYZ exploitable **a posteriori** | Amplitudes, axes, unités, fréquence réelle, filtre, configuration, durée, horodatage et qualité reconstructibles |
| V1-04 | IIS3DWB nominal **±8 g**, besoin au moins ±6 g | Configuration persistée et matériel relu, conversion vérifiée, saturation détectée ; forte excitation réelle à qualifier |
| V1-05 | **2 000 échantillons/s/axe enregistrés**, cible de conception approuvée ; **2–25 Hz cœur métier**, **2–700 Hz bande élargie cible validée** | Fréquence de sortie réellement vérifiée ; filtrage anti-repliement, réponse en amplitude/phase et bande utile 2–700 Hz qualifiés |
| V1-06 | Archive non irréversiblement débarrassée de la pesanteur | Préserver accélérations XYZ et DC utile pour retrait/filtrage hors ligne ; distinguer archives et indicateurs LIVE |
| V1-07 | Indicateurs LIVE RMS/crêtes vibratoires sans domination de la pesanteur, seulement si leur traitement ne gêne pas la mesure | Algorithme, coupe-bas, couverture temporelle, validité, transitoires et compatibilité protocole vérifiés |
| V1-08 | Jusqu'à six points de mesure, montage/orientation stable par campagne | Identité capteur/site/orientation, contexte opérationnel et synchronisation intercapteurs vérifiés |
| V1-09 | Arrêt, panne, coupure, support absent/plein | Préfixe durable récupérable, cause et quantité éventuelle de perte signalées ; jamais de fausse campagne complète |
| V1-10 | Données effectivement **récupérables sur PC** | Extraction adaptée au volume puis contrôle/import et analyses vérifiables |

**24 h = objectif secondaire**, après preuve 8 h. FFT embarquée facultative uniquement si CPU/RAM et continuité ne sont pas compromis ; FFT, analyse des harmoniques, classement des régimes, pondération de confort et visualisations élaborées sont principalement **sur PC**. Alarmes sophistiquées et esthétique Web sont différées.

## 3. Hypothèses NON GELÉES et décisions techniques à examiner

1. **Bande utile — D1 ARBITRÉE MÉTIER par l'utilisateur le 29/10/2026.** Le **cœur de l'analyse des vibrations structurelles est 2–25 Hz** (phénomènes ressentis lors des manœuvres/transitions) ; l'utilisateur **valide une bande élargie cible 2–700 Hz** pour conserver les harmoniques et usages secondaires possibles, sans faire des turbos ni des roulements la finalité du projet. À 500 tr/min la rotation est 8,33 Hz ; à 20 000 tr/min elle est 333,3 Hz (2× = 666,7 Hz). **2–700 Hz est l'objectif fonctionnel approuvé, pas une bande techniquement qualifiée** : les tolérances de gain, phase, bruit, atténuation des alias et transition du filtre restent à définir en R1. À 2 000 Hz, Nyquist = 1 000 Hz théoriques ; il ne garantit pas seul 700 Hz utiles. Les signaux archivés conservent leurs basses fréquences/DC et le filtre passe-haut d'indicateurs est distinct. La priorité d'effort métrologique reste 2–25 Hz.
2. **Fréquence exacte.** L'IIS3DWB est aujourd'hui utilisé à 26 667 Hz. Prendre 1 échantillon sur 13 produirait environ 2 051,3 Hz, **pas 2 000 Hz**. La décimation/ré-échantillonnage, le filtre anti-repliement et le modèle de timestamps doivent être définis et testés. Le traitement embarqué se limite au nécessaire.
3. **Volume.** Si archive compacte XYZ int16 : 2 000 × 3 × 2 = **12 000 octets/s**, **345,6 Mo/8 h** ou **1,0368 Go/24 h par capteur** hors en-têtes, CRC et journaux. Six capteurs : environ 2,07 Go/8 h au total **répartis entre les capteurs**. Le format bulk historique est **16 octets/échantillon**, soit 921,6 Mo/8 h à 2 kHz si conservé tel quel : choisir le format avant de figer la capacité et les buffers.
4. **Stockage.** Le SDMMC actuel est **non qualifié**, malgré des lectures/écritures ponctuellement correctes. Ne pas répéter les mêmes essais sur mêmes configurations sans hypothèse discriminante. Comparer en priorité une voie réellement différente (microSD SPI, flash appropriée, etc.), coût/capacité/endurance/coexistence SPI, récupération et latences.
5. **Temps/synchronisation.** Définir si l'on compare seulement les phases de conduite ou aussi la **phase instantanée** des vibrations entre capteurs : cela change fortement la précision temporelle requise. Horodatage par index + cadence + points de synchronisation, correction dérive à spécifier.
6. **Extraction.** Média amovible / USB / liaison dédiée à arbitrer. Le Modbus RTU LIVE ne doit pas être supposé suffisant pour transférer automatiquement plusieurs centaines de Mo/capteur.
7. **Installation/métrologie.** Fixation mécanique, sensibilité/bruit ±8 g, forte amplitude, température, CEM et comportement sur le navire exigent une qualification future. L'indication « Richter 5 » d'un smartphone n'est ni une mesure en g ni une référence métrologique.

## 4. Protection des acquis

Conserver la compatibilité de V1/V1.1, les historiques B3 absolus, les preuves et la persistance FRAM, les commandes B5 et la supervision existante. Le profil LIVE R0 dit explicitement « pas de soustraction DC implicite » : **une nouvelle sémantique métier nécessite une révision normative versionnée**, jamais une réinterprétation silencieuse. L'extension AC1 opérateur expérimentale de la mission Codex en cours ne vaut ni validation de production ni remplacement de ce cahier.

## 5. Références

[Ancien B3](../01_Specification_source/bloc3.md) · [B4](../01_Specification_source/bloc4.md) · [Temps B2](../01_Specification_source/bloc2.md) · [Profil LIVE R0](../01_Specification_source/LIVE_R0/PROFIL_LIVE.md) · [Roadmap historique](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md) · [État courant](ETAT_COURANT_TR2.md) · [Mission active](Echanges_Codex/MISSION_EN_COURS.md) · [Dernier rapport](Echanges_Codex/DERNIER_RAPPORT.md).

**Garde-fou** : ce cahier documente les décisions métier déjà exprimées et les questions ouvertes ; il ne gèle aucun nouveau protocole ni ne déclenche un développement.
