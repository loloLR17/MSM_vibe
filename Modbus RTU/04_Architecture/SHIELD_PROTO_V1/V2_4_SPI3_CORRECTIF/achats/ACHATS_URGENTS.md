# Achats à préparer pour deux shields corrigés

**Mise à jour après inventaire utilisateur du18/11/2026. Aucun achat lancé.** Le besoin est séparé de la BOM native, des pièces en stock et des options de montage. Aucune récupération de connecteur soudé n'est supposée. Aucun compte fournisseur, panier authentifié, paiement ou envoiCAM utilisé.

## Liste courte issue du stock réellement rapporté

| Préparation | Quantité | Pourquoi |
|---|---:|---|
| Connfly mâle `DS1021-2*40SF11` | **2barres2×40** | compléter la barre2×20 disponible ; minimum100colonnes pour les deux jeuxZio |
| Vishay `MRS16000C1002FCT00`,10kΩ1% | **10** | besoin8,stock2 ;10aupalier coûtent1,60USDHT contre2,04USDHT pour6àl'unité |
| AVX `SR205C104KAR`,100nF50VX7R | **1** | besoin2,stock1 ; optionhistorique couverte par l'enveloppeV27, réserve de courtyard et pose conservée |
| Panasonic `EEUFR1H100`,10µF50V | **1** | besoin2,stock1 ; optionhistorique D5H11/P2 couverte parV27, polarité à conserver |
| Samtec `SSW-103-01-F-D` | **2**, offre/délai à confirmer | aucun reçu ; référence finale prévue, ancienne adaptation1×3 distincte |

Les quatre premières lignes TME représentent **3,76USDHT**, hors TVA/transport et horsSamtec :2×0,60+10×0,16+0,65+0,31. Ce sous-total n'est **pas** le coût TTC complet livré. Les pages internationales consultées affichent USDnet ; aucune conversion/TTCFrance arbitraire. Liens, MOQ et stocks affichés dans [PREPARATION_2_SHIELDS.csv](PREPARATION_2_SHIELDS.csv) et [SOURCES_COMMERCIALES.json](SOURCES_COMMERCIALES.json).

**Ne pas racheter** les femelles1×12 (6disponibles/4nécessaires) et1×8 (6/4). Les quatre1×10 disponibles couvrent J6/J7 au niveau des enveloppesV27, avec la condition de neutraliser le contact10 lors du futur montage sur9trous. Aucun geste physique n'est demandé ou exécuté dans cette mission. Neltron1×9 `2212S-09G-85` reste la référence native ; Harwin1×9 `M20-7820946` reste une alternative sans stock/délai fraîchement confirmé.

La barre existante2×20 peut fournir deux2×10. Chacune des deux nouvelles2×40 peut fournir2×8+2×15+2×17 : exactement100colonnes utiles au total, **aucune réserve de découpe**. Cette optimisation suppose des coupes entre colonnes sans en sacrifier ; une troisièmebarre de secours0,60USDHT reste une option facultative, pas une quantité indispensable inventée.

Ne pas racheter les Nucleo, FRAM, lecteursSD, kitscapteur/MKIGIBV2, Click ou cartesSD pour cette correction. Ils restent réutilisables sans dessoudage de leurs broches, avec leurs données préservées ; leur santé complète ou le futur montage intégré ne sont pas qualifiés par cette mission. Pièces imprimées/visserieV2.7 déjà prévues, pas de nouvelle impression ni achat mécanique automatique.

## J5 : correction factuelle et priorité d'approvisionnement

L'utilisateur précise : **Samtec non reçus ; le J5 réellement testé utilise deux tronçons femelles1×3 côte à côte issus des1×10**. Les deux tronçons soudés ne sont pas comptés comme stock libre. La preuve RX/TX/Modbus du18/11 reste valable pour cet assemblage ; elle ne prouve pas qu'unSSW était monté, ni la profondeur de contact/tenue mécanique de cette adaptation. Aucun rapport historique n'est réécrit.

Le nouveau projet conserve le **SSW-103-01-F-D nominal**, dont la profondeur utile est documentée ; ne pas remplacer par une autre2×3 seulement parce que sa hauteur ressemble. Le corps constructeur complet a été contrôlé dans l'assemblageV27, sans collision, avec un budget d'insertion nominal4,48mm dans la plage3,68–6,35mm. Le STEP historique simplifie son corps longitudinal de0,225mm par extrémité : réserve publiée, pas de modification cachée deV27.

DigiKey a présenté une page partielle puis403 ; les stocks/prix anciens ne sont pas réutilisés comme actuels. La rechercheMouser fournit une quantité indexée928 ancienne, mais la page détaillée et le devisFrance ne sont pas vérifiables : **ni disponibilité actuelle ni arrivéeavantvendredi démontrées**. [LienDigiKeyexact](https://www.digikey.fr/fr/products/detail/samtec-inc/SSW-103-01-F-D/6678753), [lienMouserexact](https://www.mouser.com/ProductDetail/Samtec/SSW-103-01-F-D?qs=FESYatJ8odJ8MU68fe585Q%3D%3D). Les délais d'échantillonsSamtec ne sont pas maîtrisés. Ne rien dessouder pour y remédier dans cette mission.

## Échéance avant samedi21novembre2026

[Tarif public TMEFrance](https://www.tme.eu/fr/how-to-buy/7307/Moyens-et-couts-de-transport/) : économique **7,08€TTC**,2–4jours ouvrables ; expressUPSExpressSaver/DHL/FedEx **19,08€TTC** jusqu'à10kg,1–2jours ouvrables si commande avant14hCET. Estimations sousconditions de paiement/stock/destination, pas un engagement de livraison.

Une commande jeudi19 avant14h peut arriver vendredi20 au scénario1jour, **ou lundi23 au scénario2jours**. L'objectif strict « avant samedi21 » n'est donc pas garanti. L'économique n'est pas un plan fiable pour cette date. La pageFranceDigiKey consultable annonce habituellement3jours,25€deport sous75€ ; TVA duport non confirmée, aucune réception avantvendredi démontrée. Aucun fournisseur n'a reçu de commande ; aucun délaiPCB n'est promis.

## Références natives et options sans changement de géométrie

La [BOM native](BOM_NATIVE_2_SHIELDS.csv) contient les19repères inchangés ; le CSVd'achat distingue les options déjà considérées lors deV2.2/V2.7. Le contrôle [stock-options-M01.json](../preuves/stock-options-M01.json) vérifie les enveloppes nominales des1×10, AVX etPanasonic. Il ne change ni empreinte ni valeur électrique ni STL/STEP et n'est pas une qualification de tolérances réelles, d'effort de contact ou de pose. L'écart du courtyardC1 connu reste explicite ; queues/standoff et absence de contact du dixième terminal restent des gates du futur assemblage à froid.

La libérationPCB attend toujours la revue indépendanteE-01/M-01 deChatGPT puis la décisionutilisateur ; le ZIP reste CANDIDAT_NON_LIBERE.
