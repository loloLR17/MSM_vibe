# R4-T — retraite atomique monotone et qualification conditionnelle B3

Mission TR2-20261016-R4T-MONOTONIC-JOURNAL-RECOVERY-B3. Ce plan de maintenance
est limité au TR2-001, UID004A003D/38425018/20333157, ST-LINK003500463235510B37333439.
Il ne définit aucune récupération produit automatique. R0/V1 et les gardes R4-M/P
restent inchangés. Le STARTED historique4/5/run5 reste indécidable et archivé.

## Plan exact avant toute écriture

Préconditions : Flash M0162Mio SHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,
FRAM262144 SHA14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9,
capteur arrêté vérifié ; backups entiers récupérables. Les lectures fraîches du
16/10/2026 à22:00:20UTC correspondent aux sauvegardes R4-S.

La capture exige le SHA entier, UID du montage, root39/payload byte-identique,
epoch4/previous3/gen7/flags0, INTENT authentique run5/config3/gen3, dernier
terminal24=START4/run4, STARTED26=START5/run5 et les cinq slots/générations exacts.
Tout autre état refuse avant effet. Chaque boot commence CLOSED/WAIT ; aucun
UART/SD/ingress normal, aucun déclenchement par défaut.

La solution conserve le format et utilise le vrai moteur R2, sans seedrecord :

1. Après token explicite de sauvegarde vérifiée, retirer **seulement le slot3**
   (224 octets) dans la candidate RAM, par sept writes logiques de32octets.
   **Ne pas committer cette isolation séparément.** Conserver tous les autres
   reçus, l'epoch, les configurations et les128octets INTENT. Conserver dans le
   contexte fermé le record26 authentique, toujours STARTED/status0/result0.
2. Le frontend de maintenance retire cet actif de son état volatile uniquement,
   conserve `store.generation=26`, puis appelle le vrai
   `live_command_engine_submit(RENEW epoch4/id65535/code12)` avec paramètres nuls.
   R2 produit réellement RESERVED27 et publie ce record **dans la même enveloppe
   atomique** que la retraite du slot3. Aucun état durable de génération0/24 :
   root39/journal26 ou root40/journal27, jamais une candidate non publiée.
   Ce privilège de retraite appartient à la maintenance fermée explicitement
   autorisée ; les préconditions du moteur produit ne sont pas affaiblies.
3. Les trois vrais `step` R2 produisent STARTED28, transition epoch5 liée à29,
   puis COMPLETED30/4/0. Les générations du root augmentent39→40→41→42→43,
   l'epoch4→5, le runfloor5 reste5. Aucun champ epoch/run n'est écrit directement.
4. Ouvrir la suite de qualification seulement après terminalisation réelle,
   flags0, INTENT128 inchangé ; les cinq anciennes clés sont refusées24 sans
   écritures. Le prochain START consomme run6. Aucun reçu pour STARTED5 fabriqué.

Toutes écritures physiques viennent des commits du média existant, exclusivement
LIVE[122880,245760), payload52298+header64+root64 par barrière, quanta32 et
relectures/CRC avant publication. Historique[0,122880) et tail[245760,262144)
sont comparés intégralement. Aucun effacement physique arbitraire, nouveau format
ni slack réaffecté. Le token est consommé avant staging, y compris en échec ;
échec invalide le frontend/store et déclenche STOP, sans deuxième isolation.

## Coupures, fermeture et limites de reprise

568 budgets réels sur copie authentique : quatre barrières, payload strides,
chaque octet header/root ; monotonie avant/après reboot, INTENT/config/floors et
protections contrôlés, aucun dispatch acquisition. SHA altérés refusés. Nouvelle
suite ciblée PASS22,86s ; même suite O2/ASan/UBSan PASS13,41s. Ancien cœur R2 inchangé
et ses141tests R4-S acquis, non relancés. ARM dédié et canonique vérifiés séparément.

Un boot maintient CLOSED même si les stores récupèrent un état format-valide.
RENEW RESERVED sans STARTED peut produire le vrai résultat28 du moteur ; RENEW
STARTED sans transition reste indéterminé9 ; transition prouvée peut être
terminalisée par le vrai recovery. **Jamais de28/29 fabriqué pour STARTED5**.
Ces issues sûres ne garantissent pas une poursuite après toute coupure :
inspection et nouveau plan exact nécessaires, sans retry automatique ni
réisolation. Les modèles conservent cette limite au lieu de forcer une réussite.
Le pont RESERVED27 n'est jamais exposé à un firmware LIVE générique : avant
retraite d'epoch5, l'ancienne clé4/5 ne doit pas pouvoir être réadmise.

## Probe physique conditionnel et état final

Profil explicite R4T_RECOVERY+R4S/R4R/R4P/R4M/R4, NON-PRODUCTION. Trigger13 et
token de sauvegarde exécutent seulement la maintenance ; checkpoint à l'arrêt
capteur, snapshot FRAM entier comparé au modèle RENEW. La phase physique suivante
exige un second token/trigger14 distinct. Aucun START avant ce contrôle.

La composition réutilise provider/suivi compteur-DWT R4-S, vraie FIFO, métrologie
R3-B, B3/RTU RAM et R2/FRAM. Calibration nécessaire au nouveau run, sans répétition
du diagnostic readonly dix secondes. Les traces horloge saturent leur stockage128
mais conservent le compteur et la dernière bracket : un dépassement de capacité
de trace ne devient pas une faute de mesure pendant les longs commits R2.

Vrai START5/1/run6, service10s puis vrai STOP5/2 ; bornes work1000us/service2500us/
gap5000us inchangées, erreur→STOP sûr. Observer N4096, V/F, âge/contexte/UTCabsente,
ADU CRC/cohérence, STOP/HELD et oracle brut indépendant. N supérieurs seulement
si la marge et la couverture le permettent. Aucune liaison RS485 électrique,
WCET faute HAL/IRQ/CPU/pile, endurance ou qualification R4global/R6 annoncée.

Après essais, snapshot FRAM final intégral et audit sans rollback d'identité ;
restaurer Flash M016 seulement, par pages normales + relecture2Mio, capteur arrêté,
GDB détaché. Aucun SD/HP/câblage/OptionBytes/RDP/mass erase. Les résultats physiques
et SHA exacts de l'image seront ajoutés au rapport, sans préjuger du succès.
