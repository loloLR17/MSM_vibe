# AC1 v1 — extension diagnostique expérimentale de l’opérateur

Mission `TR2-20261028-ACCEL-FS8G-VIBRATION-AC`, addendum main36fd068 relu. **Décision explicite utilisateur du28/10/2026 :** «J’autorise l’extension diagnostique expérimentale AC1 distincte, sur les registres8000–8063, exclusivement pour le profil opérateur NON-PRODUCTION. Cette autorisation ne modifie pas le profilLIVE R0 gelé, ni la signification des registresB3 existants.» Le périmètre et la revue normative future demeurent ceux de cette décision. Ce document versionne la nouvelle interface expérimentale ; il ne modifie pas les spécifications gelées.

## Activation et limites de contrat

Le modèle canonique de registres ne contient aucune adresse8000..8063 (`test_live_ac1_wire` vérifie aussi que cela reste vrai). Les serveurs V1 et LIVE sans activation explicite conservent exception02. Le build opérateur NON-PRODUCTION seul active ce bloc FC03 ; FC10 y reste refusé. L’identité B0 persistante n’est pas réécrite. La lecture PC nécessite `--ac1-experimental` avec le profil partiel `LiveDiagnostic`, signature/version AC1 contrôlées ; aucune découverte opportuniste sur un autre profil. Ni nouvelles capacités B0 réservées, ni anciennes valeurs B3 réinterprétées. Les métriques AC1 sont un objet `DynamicAcceleration` distinct dans les observations JSON/SQLite/API ; les anciennes observations gardent null, sans migration des valeurs historiques. Une adoption dans un protocole normatif demanderait une revue/décision distincte.

## Traitement continu et sens physique

Chaque axe brut est converti en micro-g avec les sensibilités existantes61/122/244/488µg/LSB. L’estimateurDC `d[n]=d[n−1]+α(x[n]−d[n−1])` démarre au premier échantillon observé de chaqueSTART ; le signal dynamique est `y[n]=x[n]−d[n]`, **après** actualisation deDC. CalculQ16micro-g, αQ30=505863, soit `round((1−exp(−2π·2/26667))*2^30)`. Ni flottant ni allocation sur cible. L’estimateur est actualisé sur **tous** les XYZ, y compris entre les fenêtres, pour ne pas transformer les pauses du calcul en changement de filtre.

Il s’agit d’un passe-haut du premier ordre, coupure **nominale2Hz** pour la cadence configurée26667Hz ; pas une preuve d’étalonnage SI de l’horloge. Gain discret `|(1−α)(1−exp(−jω))/(1−(1−α)exp(−jω))|` : environ0,9948 à20Hz,0,99897 à50Hz,0,99976 à1000Hz. La bande utile recommandée commence à20Hz pour cette qualification de calcul ; les vibrations lentes sont atténuées, pas considérées nulles physiquement. Ni fréquence maximale capteur ni bande machine n’est inventée : anti-aliasing/capteur/montage et campagnes réelles restent à qualifier séparément.

Préparation26667échantillons (~1s), puis fenêtresN/P sur la grille temporelle duSTART. PourN4096/P2000ms, première fenêtre dynamique sur la grille2s, pendant laquelle les calculs absolus existent déjà. START réinitialise estimateur et validité ; aucune compensation de boot figée dans une orientation. Une rotation lente ou une accélération réelle soutenue peut être filtrée, une rotation rapide peut transiter dans le signal : **pas de suppression universelle de gravité en mouvement**, ni étalonnage du biais/sensibilité. STOP conserve une dernière mesureHELD noncourante ; faute, rail brut ou perte d’autorité invalident AC1, sans désactiver les gardes du FIFO.

RMS/crêtes sont calculés sur les **échantillons dynamiques signés**, arrondis au mg le plus proche avant les statistiques partagées :

- RMSaxe=`isqrt(mean(yaxe²))` ; RMSglobal=`isqrt(mean(yx²+yy²+yz²))`.
- Crêteaxe=`max(abs(yaxe))` ; crêteglobale=`isqrt(max(yx²+yy²+yz²))`.

La crête globale est le maximum d’un vecteur réellement observé, pas la combinaison de maxima d’instants différents. Le retraitDC n’est ni `RMS−1000`, ni une reconstruction depuis les seuls snapshots B3. Le DC publié est celui de fin de la fenêtre dynamique, en micro-g signés. Les statistiques64bits restent bornées même pour les inversions de signe des quatre calibres ; rails±32768/32767 refusés par la garde existante. La sensibilité±8g est244µg/LSB, quatrefois celle de±2g : moindre résolution brute, plage par axe accrue ; aucune affirmation de bruit réel ou de couverture physique±6g ne se déduit des fixtures.

## Registres8000–8063, FC03 uniquement

Entiers MSW/LSW comme l’existant ; U64 de poids fort à faible. Pas d’UTC de fenêtre inventée. Tous les réservés restent0. SnapshotRAM immuable ; le callback de preuveFS copie uniquement un readback physique déjà réalisé par le propriétaireFIFO, jamais de SPI/FRAM dans le traitement PDU.

|Offset depuis8000|Type|Champ|
|---|---|---|
|0|U16|magic4143hex («AC»)|
|1|U16|version1|
|2|U16|flags : valid1/running2/fresh4/held8/FSprouvé16/préparation32|
|3|U16|méthode1 : passe-haut complémentaire1pôle/DC2Hz|
|4|U16|codeFS de la configuration de la mesure :0/1/2/3→±2/4/8/16g|
|5|U16|CTRL1_XL réellement vérifié durant acquisition, si flag16|
|6–7|U32|cadence configuréeHz|
|8–9|U32|coupure nominalemillihertz2000|
|10–11|U32|préparation échantillons26667|
|12–15|U64|runID propre àAC1, non déduit deB3|
|16–17/18–19|U32|configID / génération de cette mesure|
|20–21|U32|séquence du calcul dynamique|
|22–23/24–25|U32|échantillons de fenêtre / durée observéems|
|26–27|U32|âge de valeur observéms, saturéU32|
|28–29/30–31/32–33|I32|DCestiméX/Y/Zmicro-g|
|34–35/36–37/38–39/40–41|U32|RMSdynamiqueglobal/X/Y/Zmg|
|42–43/44–45/46–47/48–49|U32|Crêtedynamiqueglobale/X/Y/Zmg|
|50–51|U32|compteur de readbackFS, saturéU32|
|52|U16|axes sélectionnés, masque1..7|
|53–63|U16|réservés0|

Le bitFSprouvé exige le même run et le même codeFS que le dernier readbackCTRL1_XL du propriétaire. Octets attendus pendant acquisition :A0/A8/AC/A4 pour±2/4/8/16g. À l’arrêt, il décrit le dernier readback **de cette acquisition**, pas une lecture actuelle d’ODR allumé. Si pas de preuve, le PC ne présente pas «plage effectivement prouvée». Fresh exige valid/running et âge≤2P ; HELD exige valid et arrêté. Les temps du filtre, du calcul, de réceptionPC et de l’UTC inconnue sont distincts.

## Activation durable de ±8g

`--live-command apply8g` réutilise le handofftokenisé, verrouopérateur, portexclusif, gardesB0/B3/B5 et ledgeractuel avant touteécriture. Il copie la configurationactive dont leCRC est vérifié, ne change queFS=2, prépare les seuls champsB4autorisés avec nouveauconfigID/CRC, relit le staging, puis soumet le vraiAPPLY(code1) sans paramètre inventé. Seul terminal+readbackactif/CRC/étatarrêté prouve l’effet ; aucun retry. Aucun writeFRAM brut ou rollback.

Le contrôle opérateur au recovery exige toujours autoritéactive/CRC/identité, étatR2terminal sans actif ni indétermination, mais n’impose plus les seuls identifiants historiquesconfig3/gen3. La calibration temporelle utilise aussi leFS de cette configuration ; elle ne repasse pas implicitement à±2g après le reboot d’une configuration±8g. Imagec36initiale préservée ; elle ne doit pas être restaurée aveuglément après une évolution persistante qu’elle refusait au boot. L’image corrigée récupérable conserve les mêmes autorités persistantes.

## Qualification attendue, distincte de l’implémentation

TestsC indépendants : constantes, warmup/START/STOP/faute, transfert analytique20/50/1000Hz et6500mgà100Hz, quatreFS/inversionsde signe/rails, blocopt-in/borne/readonly/noncollision. Tests.NET : parsing/type/FSphysique/qualité, transformationcomplète des vecteursR0/CRCindépendant, observation séparée/persistanceJSON, compatibilité sansopt-in. TestsJS : absenceAC surarchive, qualité propre, rupturesrun/config/FS/filtre etCSVdistinct.

**Ces tests ne valent pas qualificationphysique.** Celle-ci doit prouver configuration±8g durable, CTRL1_XL réel, métriquesAC1 réelles au repos, B3 absolu distinct, chaîneCOM4→SQLite/API→Edge, STOP disponible et conservationdesdonnées. Les résultats effectivementobservés, SHAetlimites sont dans DERNIER_RAPPORT.md ; ni excitationmachine±6g ni mesurede transfertmécanique ne sont fabriquées à partir d’un replaylogiciel.
