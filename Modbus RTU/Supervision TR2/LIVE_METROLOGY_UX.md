# Accélération absolue et présentation LIVE

Mission `TR2-20261019-DEMO-METROLOGY-UX`, exécutée le 28 octobre 2026. L’utilisateur confirme que le dépassement de la date du 20 octobre ne change pas l’objectif technique. Cette note n’amende ni V1 ni LIVE R0.

## Ce que mesurent les nombres actuels

La chaîne exécutée c36c971 utilise `live_consume` → `fifo_event_time_consume` → `live_metrology_sample`. Chaque entier signé XYZ est converti en mg, **avant** accumulation des carrés et des maxima, avec arrondi au mg le plus proche, demi-valeurs à l’opposé de zéro. `iis3dwb_units.h` donne 61/122/244/488 µg/LSB pour les codes logiques 0/1/2/3, soit ±2/4/8/16 **g**. Les encodages du registre ST ne sont pas ces codes logiques : la table `scales` de `fifo_live.c` fait la correspondance. Chaque démarrage vérifie le readback `CTRL1_XL` avec le FS attendu, puis `fifo_live_observe` le contrôle pendant l’acquisition.

`window_statistics.c` accumule `x²`, `y²`, `z²`, leur somme et les maxima. `indicator_calculator.c` calcule par racine entière :

- RMS global = `isqrt((Σx² + Σy² + Σz²) / N)` ; RMS axe = `isqrt(Σaxe² / N)`.
- Crête globale = `isqrt(max(x²+y²+z²))` ; crête axe = `max(abs(axe))`.

**Aucune moyenne par axe n’est soustraite.** Un capteur immobile peut donc montrer un RMS proche de 1 000 mg, avec une répartition XYZ dépendant de l’orientation. Cette composante constante est compatible avec la gravité ; sa mesure n’est pas, à elle seule, un étalonnage SI ni une mesure de vibration pure.

`s_calibrate` observe pendant deux secondes le compteur du capteur et des brackets DWT. Il construit une correspondance temporelle et en contrôle le résidu ≤100 µs. Il ne calcule aucun biais d’accélération, aucune orientation ni compensation de pesanteur. Les alarmes existantes sont appliquées aux grandeurs absolues : des seuils de démonstration à zéro peuvent donc expliquer un avertissement B3 sans démontrer une anomalie vibratoire. Ils ne sont pas modifiés ici.

## Vérification hors ligne réutilisant les acquis

Les deux fenêtres physiques de 4 096 XYZ du 19 octobre sont réanalysées sans répéter les essais. La conversion entière reproduit exactement les premiers résultats mémorisés :

|Fenêtre historique|RMS G/X/Y/Z (mg)|Crêtes G/X/Y/Z (mg)|Norme de la moyenne XYZ (mg)|RMS centré global, diagnostic hors ligne (mg)|
|---|---|---|---|---|
|Cycle 1|962 / 955 / 102 / 61|1000 / 991 / 135 / 103|962,405|19,411|
|Cycle 2|962 / 955 / 101 / 62|1002 / 992 / 145 / 106|962,698|19,903|

Aucun échantillon à −32768 ou +32767 dans ces fenêtres. Ces deux diagnostics centrés ne sont ni des valeurs B3 corrigées, ni des mesures de vibration calibrées : bruit, quantification, fixation et composantes temporelles non séparées restent présents. Le traitement normatif ne change pas.

Le snapshot SQLite natif Windows du début de mission contient 24 337 observations, intégrité `ok` ; les 5 569 observations V/F historiques ont des RMS globaux 951–2100 mg. Ce maximum vectoriel n’est pas une preuve de saturation d’un axe. Ni la conversion, ni B3, ni le graphe existant (`0..max(points)`) ne montrent de plafond à 2 mg. Sans période/capture originale du constat utilisateur, son origine précise n’est pas rétrodéduite. Une variation de quelques mg autour d’environ 1 g est distincte de la plage matérielle. Les observations physiques nouvelles et le FS relu sont consignés dans le rapport de mission.

## Confirmation physique du 28 octobre

Un seul nouveau cycle utile à cette mission : START5/24/run17 (40,012s) puis STOP5/25 (40,060s), terminaux4/0 et effets B3 réels. La première fenêtre brute4096, extraite après STOP, reproduit exactement le premier B3 reçu par le contrôleur : RMS G/X/Y/Z **954/950/88/12 mg**, crêtes **1002/996/126/43 mg**, zéro échantillon aux rails. MoyenneXYZ −950,281/−87,388/+2,639mg, norme954,294mg ; diagnostic RMS centré global19,391mg, toujours hors mapping normatif.

Au point d’arrêt sûr `live_runtime_step`, FIFO inactive et scopes depth0/servicingfalse, WHO_AM_I=7B et CTRL1_XL=00 sont relus : FS00=±2g, ODRoff. Le calibre pendant l’acquisition est étayé par la configuration autoritative3/gen3, `fifo.full_scale=0` et les gardes de readback physiques inchangées exécutées au START/poll ; aucune lecture GDB n’a interrompu le FIFO. Flash147024octets exactement9bbe…0981, zéro reset/flash, FRAM finale3c56ea55…3a2b5/journal126/root139/run17, préfixe historique et queue exacts à la référence97f43a du19/10. B0/B3 physiques reçus après détachement sans reset.

Le service était pausé pendant la commande START : le premier B3seq1 est dans la trace du contrôleur, **pas** réinjecté artificiellement dans SQLite. La suite SQLite/API/DOM/CSV porte sur les fenêtres réelles ultérieures du même run ; le rapport distingue ces deux correspondances.

## Présentation et données inchangées

Les six onglets LIVE et le parc mettent les valeurs absolues en mg, leur qualité et leur ancienneté avant les diagnostics. L’âge d’une dernière mesure connue est calculé depuis **sa propre réception**, distincte de l’âge du dernier échange, qui peut contenir une mesure invalide ou aucune mesure. L’âge B3 transmis est également affiché séparément : une nouvelle réception HELD ne rajeunit pas la mesure sous-jacente. Une valeur absente reste « — ». STOP/HELD reste distinct d’une panne ou de la sévérité B3 transmise.

Les courbes gardent les observations réelles, ruptures, identités, séquences répétées et CSV brut. Par défaut, l’axe Y commence à zéro avec des graduations réelles. Le mode « Détail variation » autorise une origine non nulle **annoncée explicitement** ; il ne soustrait rien aux mesures. Le texte min/max/variation décrit les valeurs du lot, jamais le calibre du capteur. Les boutons et contrôles persistent pendant le polling ; aucune nouvelle source Modbus, API d’écriture ou cache métier n’est ajoutée.

Les pages, CSS et JS sont des fichiers `wwwroot` servis par `UseStaticFiles`. Une publication complète candidate contenant les mêmes exécutables est contrôlée sous Windows/COM99 avant mise à jour des seuls assets du service fonctionnel. Les DLL ne sont ni recompilées, ni mélangées. Chaque asset d’origine est sauvegardé ; la signature durable reste un chantier distinct.

## Proposition métrologique distincte — décision structurante requise

Si une grandeur vibratoire débarrassée de DC est souhaitée, spécifier **un nouvel indicateur versionné**, sa bande passante, sa fenêtre et ses règles de validité. Une piste de travail est `μi = Σai/N`, puis `RMS_AC = sqrt(Σi(mean(ai²)−μi²))`, avec précision suffisante avant quantification. La crête centrée requiert aussi la définition et l’accès aux échantillons de la fenêtre ; elle ne se déduit pas des seules crêtes absolues.

La moyenne sur N échantillons retire toute constante sur cette fenêtre, y compris une accélération réelle soutenue ; une inclinaison ou un déplacement lent peut rester mêlé au signal. Ce n’est pas un étalonnage du zéro. Un filtre passe-haut serait un autre contrat avec coupure, transitoire et phase à spécifier. Soustraire 1 000 mg au RMS global est mathématiquement incorrect. Aucun historique, seuil ou registre B3 ne doit être réinterprété silencieusement. Augmenter FS sans saturation démontrée dégrade la résolution et n’est pas justifié par les preuves présentes.

## P2 — START/STOP Web : conception différée, recours conservé

Le scope P1 est livré en lecture seule. START.cmd/STOP.cmd restent les actions physiques qualifiées. Aucun bouton factice ou endpoint d’écriture nouveau n’est ajouté.

L’intégration future doit utiliser une unique autorité dans la même composition Service : demande volontaire authentifiée sur localhost, protection CSRF et contrôle d’origine, adaptateur de commandes partagé avec le contrôleur éprouvé. L’admission vérifie B0/B3/B5, disponibilité transport/profil, pause tokenisée avec acquittement après libération réelle et ouverture exclusive du port ; allocation monotone durable dans **le ledger actuel avant FC10**. Une demande Web ne vaut pas un effet : afficher demandé/en cours/terminal+effet/incertain. Aucune réémission automatique après timeout, aucun second maître ou accès Modbus depuis le navigateur. Les preuves physiques devront couvrir START depuis STOP et HELD, refus transport perdu/conflit, STOP lent et reprise incertaine ; tests de sécurité et nouveau gate Windows obligatoire si le Service est recompilé. Dette ouverte, sans changement de l’autorité S7.
