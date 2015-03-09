import pandas as pd
import numpy as np
import csv
import sys
import time
def helpfuction():
	print '-------------------------------------------'
	print "Please type <python name.py 0/1/2>"
	print '0 reprents user-based recommendation system'
	print '1 reprents item-based recommendation system'
	print '2 reprents combined recommendation system'
	print '-------------------------------------------'
	return 0

def assign_to_set(df):
    sampled_ids = np.random.choice(df.index,
                                   size=np.int64(np.ceil(df.index.size * 0.02)),
                                   replace=False)
    df.ix[sampled_ids, 'for_testing'] = True
    return df

def compute_rmse(y_pred, y_true):
    """ Compute Root Mean Squared Error. """
    return np.sqrt(np.mean(np.power(y_pred - y_true, 2)))

def evaluate(estimate_f):
    """ RMSE-based predictive performance evaluation with pandas. """
    # allData_test = allData_test.pivot_table('rating', cols='movie_id', rows='user_id')
    ids_to_estimate = zip(allData_test.user_id, allData_test.movie_id)
    estimated = np.array([estimate_f(u,i) for (u,i) in ids_to_estimate])
    real = allData_test.rating.values
    return compute_rmse(estimated, real)

def pearson(s1, s2):
    """Take two pd.Series objects and return a pearson correlation."""
    s1_c = s1 - s1.mean()
    s2_c = s2 - s2.mean()
    return np.sum(s1_c * s2_c) / np.sqrt(np.sum(s1_c ** 2) * np.sum(s2_c ** 2))

def cosine(s1, s2):
	return np.sum(s1 * s2) / np.sqrt(np.sum(s1 ** 2) * np.sum(s2 ** 2))

def jaccard(s1, s2):
	s1 = s1.notnull()
	s2 = s2.notnull()
	return float((s1&s2).sum()) / float((s1|s2).sum())





class CollabFiltering:
    """ Collaborative filtering using a custom sim(u,u'). """

    def learn(self,mode):
        """ Prepare datastructures for estimation. """
        if mode == 'user_based':
        	self.all_user_profiles = allData.pivot_table('rating', rows='movie_id', cols='user_id')
        elif mode == 'item_based':
        	self.all_user_profiles = allData.pivot_table('rating', rows='user_id', cols='movie_id')
        else:
        	self.all_user_profiles = allData.pivot_table('rating', rows='movie_id', cols='user_id')


    def estimate_userbased(self, user_id, movie_id, simfunc = 0):
        """ Ratings weighted by correlation similarity. """
        ratings_by_others = allData_train[allData_train.movie_id == movie_id]
        # ratings_by_others = allData_train[allData_train.index == movie_id]
        if ratings_by_others.empty: return 3.0
        ratings_by_others.set_index('user_id', inplace=True)
        their_ids = ratings_by_others.index
        their_ratings = ratings_by_others.rating
        their_profiles = self.all_user_profiles[their_ids]
        user_profile = self.all_user_profiles[user_id]
        sims = their_profiles.apply(lambda profile: pearson(profile, movie_profile), axis=0)
        # sims = their_profiles.apply(lambda profile: cosine(profile, movie_profile), axis=0)
        # sims = their_profiles.apply(lambda profile: jaccard(profile, movie_profile), axis=0)
        ratings_sims = pd.DataFrame({'sim': sims, 'rating': their_ratings})
        ratings_sims = ratings_sims[ ratings_sims.sim > 0]
        global count
        print 'return number {} test'.format(count)
        count += 1
        if ratings_sims.empty:
            return their_ratings.mean()
        else:
            return np.average(ratings_sims.rating, weights=ratings_sims.sim)

    def estimate_itembased(self, user_id, movie_id,simfunc=0):
        """ Ratings weighted by correlation similarity. """
        ratings_of_other_item = allData_train[allData_train.user_id == user_id]
        if ratings_of_other_item.empty: return 3.0
        ratings_of_other_item.set_index('movie_id', inplace=True)
        their_ids = ratings_of_other_item.index
        their_ratings = ratings_of_other_item.rating
        their_profiles = self.all_user_profiles[their_ids]
        movie_profile = self.all_user_profiles[movie_id]
        sims = their_profiles.apply(lambda profile: pearson(profile, movie_profile), axis=0)
        ratings_sims = pd.DataFrame({'sim': sims, 'rating': their_ratings})
        ratings_sims = ratings_sims[ ratings_sims.sim > 0]

        if ratings_sims.empty:
            return their_ratings.mean()
        else:
            return np.average(ratings_sims.rating, weights=ratings_sims.sim)


if __name__ == "__main__":

	unames = ['user_id', 'gender', 'age', 'occupation', 'zip']
	users = pd.read_table('users.csv',
                      sep=',', header=None, names=unames)

	rnames = ['user_id', 'movie_id', 'rating', 'timestamp']
	ratings = pd.read_table('ratings.csv',
                        sep=',', header=None, names=rnames)

	mnames = ['movie_id', 'title', 'genres']
	movies = pd.read_table('movies.csv',
                       sep=',', header=None, names=mnames)
	count = 0
	allData = pd.merge(pd.merge(ratings, users), movies)

	unames_CV = ['user_id', 'gender', 'age', 'occupation', 'zip']
	users_CV = pd.read_table('users_CV.csv',
                      sep=',', header=None, names=unames_CV)
	mnames_CV = ['movie_id', 'title', 'genres']
	movies_CV = pd.read_table('movies_CV.csv',
                      sep=',', header=None, names=mnames_CV)
	allData_test = pd.merge(pd.merge(ratings,users_CV), movies_CV)



	allData_train = allData
	

	len(sys.argv)
	if len(sys.argv) == 1:
		helpfuction() 
		exit(0)
	reco = CollabFiltering()
	
	options_learn = {'0': reco.learn('user_based'),
				'1': reco.learn('item_based'),
				'2': reco.learn('combine')
				}
	options_learn[sys.argv[1]]
	start = time.clock()
	with open('toBeRated.csv', 'r') as csvinput:
		with open('result_pearson_itembased.csv', 'w') as csvoutput:
			writer = csv.writer(csvoutput)
			reader = csv.reader(csvinput)
			count = 0
			u_m_p = []
			for row in reader:
					predict = reco.estimate_itembased( int(row[0]), int(row[1]))

					row.append( predict )
					u_m_p.append(row)
			writer.writerows(u_m_p)
    # print 'RMSE for CollabFiltering: %s' % evaluate(options_estimate[sys.argv[1]])
	end = time.clock()
	duration = end - start;
	print 'spend %s time' % duration
